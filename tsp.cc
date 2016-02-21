/*
  		CS486 Assignment 1
  		Hu Qi, 20549741
  		
  		Question 3
  		Main Solution
  		
  		1.This implementation will solve the TSP problem of size 10 within 15 minutes
  		2.Notice that to run this program, 
  		  it is necessary to pass in the name of the file as the first parameter/   eg.  "./tsp instance_1.txt"
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <list>
#include <sstream>
#include <cstdio>
#include <map>
#include <cstdlib>
#include <cassert>
#include <string>
#include <cmath>

using namespace std;

struct node{
  char name;		//identify the name of the city
   node* parent;	//where do I come from
   double gn;
   double fn;
   int depth; 		//depth is used to determine how far have this node gone.
} root;

//number of nodes expanded
int counter;

//map information, city name and its coordinates
map< char, pair<int, int> > mapinfo;

//full city list
vector<char> citylst;


//distance btw cities, implemented as a two dimentional array for fast access
#define row 36
#define col 36
double dist[row][col];

//open list is used to keep track of all the nodes that has been explored, and can be further expanded (fringe)
vector<struct node*> open;

//close list is not used for this type of problem
//vector<struct node*> close;


//read in the name of the file
char* filename;


//read in the map information
void construct_map(){
   char name;
   int x;
   int y;
   int num;
   
   pair<int, int> temp;
   
   ifstream infile;
   //setting the name of the file
   infile.open( filename );
   
   infile >> num;
   
   while(!infile.eof()){
      infile >> name >> x >> y;
      temp = make_pair(x, y);
      mapinfo.insert (pair<char, pair<int, int> >(name, temp) );
   }
   
   for (map<char , pair<int, int> >::iterator it = mapinfo.begin(); it != mapinfo.end(); ++it){
      citylst.push_back(it->first);
   }
}


//calculate the distance btw any two cities
void create_distance(){
   int size = mapinfo.size();
   for (map<char , pair<int, int> >::iterator it = mapinfo.begin(); it != mapinfo.end(); ++it){
      for (map<char , pair<int, int> >::iterator it2=mapinfo.begin(); it2!=mapinfo.end(); ++it2){
         int x_diff = it->second.first - it2->second.first;
         int y_diff = it->second.second - it2->second.second;
         double diff = sqrt(pow( x_diff , 2) + pow( y_diff, 2) );
         dist[it->first - 'A'][it2->first - 'A'] = diff;
      }
   }
}

//remove an element from the vector
void vector_remove(vector<char> &temp, char str){
   int size = temp.size();
   for (int i=0; i<size; i++){
      if(temp[i] == str){
         temp.erase(temp.begin()+i);
      }
      else {}
   }
}

//check for existance in a vector
int exist(vector<struct node*>temp, char str){
   int size = temp.size();
   
   for(int i=0; i<size; i++){
      if(temp[i]->name == str) return 1;
   }
   return 0;
}
   



//used for sort() comparing two struct node*
bool mysort(struct node* node1, struct node* node2){
   return node1->fn <= node2->fn;
}



float calculate_heuristic( vector<char> rest, char current){
   int size = rest.size();
   //notice that the situation of having arrived at the very leaf of the tree and "heuristic" is simply the distance back to A.
   float current_lowest = dist[rest[0]-'A'][rest[1] -'A'];
   
   

   for(int i = 0; i < size; i++){
      for(int j =0; j < size; j ++){
         char name1 = rest[i];
         char name2 = rest[j];
         float current_value = dist[name1-'A'][name2 - 'A'];
         //in finding the smallest potential edge, 
         //we cant have the current node going back to root, or calculate dist btw same two cities
         if ((name1 == 'A' && name2 == current) || (name1 == current && name2 == 'A') || (name1 == name2)) continue;
         else if (current_value < current_lowest) {
            current_lowest = current_value;
            //cout << name1 << " " << name2 << " " << current_value <<endl;
         }
         else {};
      }
   }
   return current_lowest;
}


//find and element in the vector
struct node* find(vector<struct node*>temp, char name){
   for(int i = 0; i<temp.size(); i++){
      if(temp[i]->name == name) return temp[i];
   }
   return NULL;
}

/*
double find_gn(vector<struct node*> temp, char str){
   for (int i =0; i<temp.size(); i++){
      if (temp[i]->name == str) return temp[i]->gn;
   }
}
*/ 

node* expand(node* current){
   //cout << "currently at: " <<  current->name << endl;
   char name = current->name;
   int depth = current->depth;
   double current_gn = current->gn;
   vector<char> expandlst = citylst;
   
   /*
   if (current->parent != NULL){
      char parent_name = current->parent->name;
      //double parent_gn = current.parent->gn;
      vector_remove(expandlst, parent_name);
   }*/
   node* new_current = current;
   while(new_current->parent != NULL){ //here the parent could be NULL, in the case of root A
      char parent_name = new_current->parent->name;
      //double parent_gn = current.parent->gn;
      vector_remove(expandlst, parent_name);
      new_current = new_current->parent;
   }
   
   //remove itself and its direct parent from the expandable list because they are impossible to yield a better path
   vector_remove(expandlst, name);

   //cout<< "size of expandlst is: " << expandlst.size() << " " << expandlst[0] <<endl;
   
   double gn;
   double hn;
   double fn;
   int size = expandlst.size();
   
   //cout<< "size of openlst is: " << open.size() << " " << open[0]->name << endl;
   //cout<< "size of closelst is: " << close.size() << endl;
   
   //close.push_back(current);
   open.erase(open.begin());
   
   for (int i = 0; i < size; i++){
      char next = expandlst[i];
      //cout << "*******" << "potential node being looked at is: " << next << endl;
      
      
      /*
      if (exist(open, next) == 0 && exist(close, next) == 0) {
         gn = current_gn + dist[name - 'A'][next - 'A'];
      }
      
      if (exist(open, next) == 1 && exist(close, next) == 0){
         gn = find_gn(open, next) + dist[name - 'A'][next - 'A'];
         //cout<< find_gn(open, next) << endl;
      }
      
      if (exist(open, next) == 0 && exist(close, next) == 1){
         gn = find_gn(close, next) + dist[name - 'A'][next - 'A'];
         //cout << find_gn(close, next) << endl;
      }
      */
      
      //gn is calculated by adding up the gn from its parent and the distance from its parent to itself
      gn = current_gn + dist[name - 'A'][next - 'A'];
      //cout << current_gn << endl;
      
      //cout << name << next << " : " << dist[name - 'A'][next - 'A']<< endl;
      
      //remaining number of steps to finish traversing all the cities and return to A
      int remaining_steps = mapinfo.size() - depth;
      vector<char> rest = expandlst;
      
      rest.push_back('A');
      //cout<< "size of rest is: " << rest.size() << " " << rest[0] <<endl;
      
      //lowest step gives the lowest tree edge(gn) among the possible remaining steps if we were to take this path through expandlst[i]
      float lowest_step = calculate_heuristic(rest, next);
      hn = remaining_steps * lowest_step;
      //cout << "number of remaining steps is " << remaining_steps << endl;
      fn = gn + hn;
      
      //cout << "gn of " << expandlst[i] << " is " << gn << endl;
      //cout << "hn of " << expandlst[i] << " is " << hn << endl;
      //cout << "fn of " << expandlst[i] << " is " << fn << endl;
      
      
      
      
      
      
      //if ( exist(open, next) == 0 && exist(close, next) == 0) {
         //cout<<"I am in situation 1"<<endl<< endl;
         node* child = new node;

         child->name = expandlst[i];

         child->parent = current;

         child->gn = gn;

         child->fn = fn;

         child->depth = depth + 1;

         open.push_back(child);

      //}
      
      // else if ( exist(open, next) == 1 && exist(close, next) == 0) {
      //    //cout<<"I am in situation 2"<<endl<<endl;
      //    node* previous= find(open, expandlst[i]);
      //    //cout << next << " previously has fn of: " << previous->fn << " from " << previous->parent->name<< endl;
      //    //cout << " the newly calculated fn is " << fn << " from " << current->name << endl<< endl;
      //    assert (previous != NULL);
      //    if (fn >= previous->fn) continue;
      //    else {
      //       //cout << "path switched, better path found!!!" << endl;
      //       //cout << next << " now has parent " << current->name << " instead of " << previous->parent->name << endl;
      //       previous->parent = current;
      //       previous->depth = current->depth + 1;
      //       previous->fn = fn;
      //    }
      // }
      
      // else if ( exist(open, next) == 0 && exist(close, next) == 1) {
      //    //cout<<"I am in situation 3"<<endl<<endl;
      //    node* previous= find(close, expandlst[i]);
      //    //cout << next << " previously has fn of: " << previous->fn << " from " << previous->name<< endl;
      //    //cout << " the newly calculated fn is " << fn << " from " << current->name << endl<<endl;
      //    assert (previous != NULL);
      //    if (fn >= previous->fn) continue;
      //    else {
      //       //cout << "path switched, better path found!!!" << endl;
      //       //cout << next << " now has parent " << current->name << " instead of " << previous->parent->name << endl;
      //       previous->parent = current;
      //       previous->depth = current->depth + 1;
      //       previous->fn = fn;
      //       open.push_back(previous);
      //    }
      // }
   }
   //cout<<"I am in situation 1  position 8"<< open.back()->name <<endl<< endl;
   for(int i = 0; i < open.size(); i++){
      //cout << open[i]->name << " " << open[i]->parent->name << open[i]-> depth<<endl;
   }
   
   //sort the remaining expandable nodes in increasing order of fn, this way we are always trying out the most "promising" option first
   sort(open.begin(), open.end(), mysort);
   //cout<<"I am in situation 1  position 9"<<endl<< endl;
   //cout<< "size of openlst is: " << open.size() << " " << open[0]->name << endl;
   //cout<< "size of closelst is: " << close.size() << endl;
   //cout<< "--------------------------------------------------------------------------------------------------" << endl;
   
   
   //terminate the recursion once we have a path that have traversed all the cities
   if(open[0]->depth == mapinfo.size()
      ) return open[0];
   
   else return NULL;
   
}




node* build(){
   //initialize root with starting point A
   root.name = 'A';
   root.parent = NULL;
   root.gn = 0;
   root.fn = 0;
   root.depth = 1;
   
   //initialize counter
   counter = 0;
   
   // put A in the open list
   open.push_back(&root);
   
   ////cout << open[0]->name << endl;
   
   //if open list is empty then fail, since there are no more expandable nodes
   assert(open.size() != 0);
   
   while(true){
      //cout <<"opensize=========================="<< open.size();
      node* ans = expand(open[0]);
      counter ++;
      
      // return the first non-NULL is the optimal solution
      if(ans != NULL) return ans;
   }
   
   return NULL;
}
   
   

void printpath(struct node* last){
   int depth = last->depth;
   char lst[depth];
   cout << "The path expanded is in order: " << endl;
   
   for (int i =0; i < depth; i++){
      //cout << last ->name << " <<--- ";
      lst[i] = last->name;
      last = last->parent;
   }
   
   for (int i = depth -1; i >= 0; i-- ){
      cout << " " << lst[i] << " -->> ";
   }
   
   cout<< " FINISHED! "<< endl;
}













int main(int argc, char** argv){


	
	if(argc > 1){
		filename = argv[1];
	}
	
	
   construct_map();
   
   
   create_distance();
   
   node *ans = build();
   
   int k = ans->depth;
   //cout << "depth of tree is: " << k << endl;
   
   printpath(ans);
	
	//cout<< "size of map is: " << citylst.size() << endl;
   cout << "Number of nodes expanded is: " << counter << endl;

   return 1;
}
