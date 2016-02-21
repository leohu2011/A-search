/*
  		CS486 Assignment 1
  		Hu Qi, 20549741
  		
  		Question 3
  		Second Solution
  		
  		1.This implementation will NOT solve the TSP problem of size 10 within 15 minutes, because I have used a lot of string 		 			  comparisons, which significantly slows down the efficiency
  		2.Notice that this program defaults the file to read from is called "instance_1.txt"
  		3.However, as compared to the other implementation, this solution expands less nodes
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

//(name, ( (cost_so_far, heuristic), (x,y) ) )
map< string, pair<int, int> > mapinfo;

//distance btw any two cities
map<string, float> distlst;

//list of cities and their coordinates
vector< pair<string, pair <int, int> > > citylist;

//unvisited list of cities
vector<string> unvisited;

//fringe
vector<string> fringe;

//finding out position of the tree
vector<pair <string, struct citytree*> >position;

//keep track of the number of nodes expanded
int counter;


struct citytree{
   string name;
   string path;
   float cost_so_far;
   //citytree *parent;
   //vector<citytree* >children;
   //vector<string> visited;
   vector<string> remaining;
   vector<string> child_remaining;
}ctree;


float calculate_dist(string name1, string name2);

//read in the map information
void construct_map(){
   string name;
   int x;
   int y;
   int num;
   
   pair<int, int> temp;
   pair< float, pair<int, int> >temp2;
   pair<string, pair<int, int> >temp3;
   
   ifstream infile;
   //setting the name of the file
   infile.open("instance_1.txt");
   
   infile >> num;
   
   while(!infile.eof()){
      infile >> name >> x >> y;
      temp = make_pair(x, y);
      mapinfo.insert (pair<string, pair<int, int> > (name, temp) );
      //citylist.push_back(temp3);
      //unvisited.push_back(name);
   }
}

void construct_city(){
   for (map<string, pair<int, int> > ::iterator it = mapinfo.begin(); it != mapinfo.end(); ++it){
      string name = it->first;
      unvisited.push_back(name);
      pair<int, int> temp = make_pair(it->second.first, it->second.second);
      pair <string, pair<int, int> >temp2 = make_pair(name, temp);
      citylist.push_back(temp2);
   }
}

/*
void create_heuristic() {
   int base_x = mapinfo["A"].second.first;
   int base_y = mapinfo["A"].second.second;
   for(map< string, pair< float, pair<int, int> > >::iterator it = mapinfo.begin(); it != mapinfo.end(); ++it){
      int temp_x = it->second.second.first;
      int temp_y = it->second.second.second;
      float guess = sqrt (2 * ( abs(base_x - temp_x) + abs(base_y - temp_y) ) );
      it->second.first = guess;
   }
}
*/


float calculate_heuristic( vector<string> rest, string current){
   int size = rest.size();
   float current_lowest = distlst[rest[0]+"_"+rest[1]];
   //when size is 2, it means we are already at the return to A step for heuristic
   if (size == 2){
   	return current_lowest;
   }
   //cout << rest[0] << " " << rest[1] << " " << current_lowest << endl;
   for(int i = 0; i < size; i++){
      for(int j =0; j < size; j ++){
         string name1 = rest[i];
         string name2 = rest[j];
         string name = name1 + "_" + name2;
         float current_value = distlst[name];
         if ((name1 == "A" && name2 == current) || (name1 == current && name2 == "A") || (name1 == name2)) continue;
         else if (current_value < current_lowest) {
            current_lowest = current_value;
            //cout << name1 << " " << name2 << " " << current_value <<endl;
         }
         else {};
      }
   }
   return current_lowest;
}


   

float calculate_dist(string name1, string name2){
   int x1 = mapinfo[name1].first;
   int y1 = mapinfo[name1].second;
   
   int x2 = mapinfo[name2].first;
   int y2 = mapinfo[name2].second;
   
   float real_dist = sqrt(pow( x1-x2 , 2) + pow(y1 - y2, 2) );
   
   return real_dist;
}

void create_distance() {
   string from, to;
   string name;
   float dist;
   int size = citylist.size();
   //cout << "size of city is: "<< size<< endl;
   for(int i =0; i < size; i++){
      for (int j = 0; j<size; j++){
         from = citylist[i].first;
         to = citylist[j].first;
         name = from + "_" + to;
         dist = calculate_dist(from, to);
         //cout<< "distance calculated is: "<<name<<" is apart by: "<<dist<<endl;
         distlst.insert(pair<string, float>(name, dist) );
      }
   }
}

vector<string> vector_remove(vector<string> temp, string str){
   int size = temp.size();
   for (int i=0; i<size; i++){
      if(temp[i] == str){
         temp.erase(temp.begin()+i);
      }
      else {}
   }
	return temp;
}

/*
struct citytree{
   string name;
   string path;
   float cost_so_far;
   citytree *parent;
   vector<citytree* >children;
   vector<string> remaining;
};
*/

/*
citytree* find_position(citytree *tree, string name){
	   if (tree->path == name) return tree;
	   else {
	   	cout << "number of children of node " << tree->path << " is: " <<tree->children.size() << endl;
		   for (int i=0; i < tree->children.size() ; i++){
			   return find_position(tree->children[i], name);
		   }
	   }
	   return NULL;
}
*/

citytree* find_position(string name){
	for (int i =0; i<position.size(); i++){
		if (position[i].first == name) return position[i].second;
	}
	return NULL;
}

pair<string, string> find_next(int &cost){
	
	citytree *base = find_position(fringe.back());
	
	float current_gn = base->cost_so_far + distlst[base->name + "_" + base->remaining[0]];
	//calculate_dist(base->name, base->remaining[0]);
	
	vector<string> v = base->remaining;
	
	v.push_back("A");
	
	float current_hn = base->remaining.size() * calculate_heuristic(v, base->remaining[0]);
	//cout << "this is wrong!!!!!!!!!!!!!!!!!!!!" << endl;
	float current_sum = current_gn + current_hn;
	string current_best = base->path;
	string current_next = base->remaining[0];


   for(int i=0; i < fringe.size(); i++ ){
		string name = fringe[i];
		//cout << name << endl;
		citytree *temp = find_position(name);
		assert(temp != NULL);
		float gn = temp->cost_so_far;
		
		
		for (int j=0; j < temp->remaining.size() ; j++){

			
		   vector<string> rest;
		   rest = temp->remaining;
		   rest.push_back("A");
		   
			string next = temp->remaining[j];
			
			//cout<< "exploring potential path from: "<< fringe[i] << " to " << temp->remaining[j] << endl;
			
			float gn = temp->cost_so_far + distlst[temp->remaining[j] + "_" + temp->name];
			//calculate_dist(temp->remaining[j] , temp->name);
			
         
         int remaining_steps = mapinfo.size() - temp->path.length();
         float lowest_step = calculate_heuristic(rest, next);
         //cout << lowest_step << endl;
			float hn = remaining_steps * lowest_step;
			float sum = hn + gn;
			
			//cout << temp->name << " " << temp->remaining[j] << " " << gn << " " << hn << " "<< temp->remaining.size() <<endl;
			
			
			if (sum < current_sum){
				current_sum = sum;
				current_gn = gn;
				current_hn = hn;
				current_best = name;
				current_next = next;
			}
		}
		
	}
	
	//cout << current_best << " " << current_next << " " << current_sum << endl;
	
	cost = current_gn;
	
	return make_pair(current_best, current_next);
}
		
		
		

string build_tree(citytree &ctree){
   //initialization
	
   string start = "A";
   ctree.name= start;
   ctree.path= start;   //ctree->parent->path + ctree->name
	//ctree.parent = NULL;
   ctree.cost_so_far = 0;
   
	
   ctree.remaining = vector_remove(unvisited, start);
   ctree.child_remaining = vector_remove(unvisited, start);
	

   fringe.push_back(ctree.path);
   position.push_back(make_pair(ctree.path, &ctree));
   
	//cout << "finished initialization of fringe, fringe has size: " << fringe.size() << " " << fringe[0] <<endl;
   
   int totoalsize = 0;
	for(int i =0 ; i< unvisited.size(); i++){
	   totoalsize += unvisited[i].length();
	}
   
   while (true){
   	
   	counter++;
   	
      pair<string, string> next;
      int cost;
	   next = find_next(cost);
      
	   citytree* previous = find_position(next.first);
	   
	   citytree * child = new citytree;
	   
	   //cout << "I am HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	   //cout << next.first << " " << next.second << endl;
	   
	   child->name = next.second;
	   
	   child->path = next.first + next.second;
	   
	   child->cost_so_far = cost;
	   
	   //child->parent = previous;
	   
	   //previous->children.push_back(child);
	   //cout<< previous->path << " " << previous->children.size() << " ************************" << endl;
	   //cout << child->path << " *****************************" <<endl;
	   //cout << previous->children.back()->path << "****************************" << endl;
	   child->remaining = vector_remove(previous->child_remaining, next.second);
	   child->child_remaining = child->remaining;
	   
	   previous->remaining = vector_remove(previous->remaining, next.second);
	   
	   fringe.push_back(child->path);
	   position.push_back(make_pair(child->path, child));
	   
	   //when we have traversed all the cities, have found the optimal path. return that path
	   if (next.first.length() + next.second.length() == mapinfo.size()){
	   	return next.first + next.second;
	   }
	   
	   
	   //cout << "THE node being expanded is: " << next.first << " " << next.second << endl;
	   //cout << "size of fringe is now: " << fringe.size() << " " << fringe.back() <<endl;
	   	   
	}
	   
	      
   return "search failed";
}





int main(){
   construct_map();
   
   construct_city();
   
   //create_heuristic();
   
   create_distance();
	
	vector<string> fringe;
	string ans = build_tree(ctree);
   
   cout<< "The path is: " << ans << endl;
   cout << "Number of nodes expanded is: " << counter << endl;
   
   //cout << "size of map is: " << mapinfo.size() << endl;
   //cout << "access first element: " << mapinfo["A"].second.first << endl;
   //cout << "heuristic of D is: " << mapinfo["D"].first << endl;
   //cout << "real distance btw A and D is: " << calculate_dist("A", "D") << endl;
   //cout << "last city is called: " << citylist[35].first << endl;
   //cout << "size of city is: " << unvisited.size() << endl;
   return 1;
}







