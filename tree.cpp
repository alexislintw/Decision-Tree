#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
using namespace std;


vector< map<string,string> > tree;
vector< map<int,double> > dataset;

struct Node { 
	int idx;
	double threshold;
	int label;
	Node *left;
	Node *right;
};

struct Node* newNode(int idx, double threshold, int label) 
{ 
	struct Node* temp = new struct Node; 
	temp->idx = idx;
	temp->threshold = threshold;
	temp->label = label;
	temp->left = NULL;
	temp->right = NULL; 
	return temp;
} 

vector<string> split(string str, char delimiter) {
  vector<string> v;
  stringstream ss(str);
  string tok;
 
  while(getline(ss, tok, delimiter)) {
    v.push_back(tok);
  }
 
  return v;
}

void load_data(string file_path) {
	int count = 0;
	fstream f;
    string s;
	    
    f.open(file_path, ios::in);
    while(getline(f,s)){
        map<int,double> m;
        
        vector<string> sep = split(s,' ');
        m[0] = stod(sep[0]);
        for(int i=1; i<sep.size(); i++) {
        	vector<string> sep2 = split(sep[i],':');
        	int idx = stoi(sep2[0]);
        	double val = stod(sep2[1]);
        	m[idx] = val;
        }
        dataset.push_back(m);
    }
    f.close();
    //printf("loading data is done: %lu\n",dataset.size());
}

int get_data_fully_size(vector< map<int,double> > examples) {
	int max_idx = 0;
	for(int i=0; i<examples.size(); i++) {
		map<int,double> m = examples[i];
		map<int,double>::iterator iter;
		iter = m.end();
  		--iter;
		
		if(iter->first > max_idx) {
			max_idx = iter->first;
		} 
	}
	return max_idx + 1;
}

map<int, vector<double> > thresholds(vector< map<int,double> > examples) {
	map<int, vector<double> > thresholds;
	map<int, vector<double> > values;

	int fully_size = get_data_fully_size(examples);
	
	for(int i=0; i<examples.size(); i++) {
		map<int,double> m = examples[i];
		for(int i=1; i<fully_size; i++) {
			int key = i;
			double val = m[i];
			if(!val) {
				val = 0;
			}
			if(values.count(key)) {
				vector<double> v = values[key];
				if(find(v.begin(),v.end(),val) == v.end()) {
					v.push_back(val);
					values[key] = v;
			 	}
			}
			else {
				vector<double> new_v;
				new_v.push_back(val);
				values[key] = new_v;
			}
		}
	}
	
	map<int, vector<double> >::iterator iter;
	for(iter = values.begin(); iter != values.end(); iter++) {
		int key = iter->first;
		vector<double> vec = iter->second;
		
		sort(vec.begin(),vec.end());
		vector<double> vec2;
		
		for(int i=0; i<vec.size()-1; i++) {
			double t = (vec[i] + vec[i+1])/2;
			vec2.push_back(t);
		}
		
		thresholds[key] = vec2;
	}
	
	return thresholds;
}

vector<int> count_yn(vector< map<int,double> > examples) {
	int count_Y = 0;
	int count_N = 0;
	vector<int> arr;
	
	for(int i=0; i<examples.size(); i++) {
		map<int,double> m = examples[i];
		if(m[0] == 1) {
			count_Y ++;
		}
		else {
			count_N ++;
		}
	}
	arr.push_back(count_Y);
	arr.push_back(count_N);
	return arr;
}

vector<int> count_yn_by_factor(vector< map<int,double> > examples, int idx, double threshold) {
	int count_cY = 0;
	int count_dN = 0;
	int count_eY = 0;
	int count_fN = 0;
	vector<int> arr;
	
	for(int i=0; i<examples.size(); i++) {
		map<int,double> m = examples[i];
		if(m[idx] >= threshold) {
			if(m[0] == 1) {
				count_cY ++;	
			}
			else {
				count_dN ++;
			}
		}
		else {
			if(m[0] == 1) {
				count_eY ++;
			}
			else {
				count_fN ++;
			}
		}
	}
	
	arr.push_back(count_cY);
	arr.push_back(count_dN);
	arr.push_back(count_eY);
	arr.push_back(count_fN);
	
	return arr;
}

double confusion(int a, int b) {
	return 2 * min( a / (double)( a + b ), b / (double)( a + b ) );
}

double total_confusion(int c, int d, int e, int f) {
	return ((double)(c+d)/(double)(c+d+e+f))*confusion(c,d) + ((double)(e+f)/(double)(c+d+e+f))*confusion(e,f);
}

map<string, vector< map<int,double> > > separate_subset(vector< map<int,double> > examples, int idx, double threshold) {
	map<string, vector< map<int,double> > > subset;
	vector< map<int,double> > v1;
	vector< map<int,double> > v2;
	
	for(int i=0; i<examples.size(); i++) {
		map<int,double> m = examples[i];
		if(m[idx] > threshold) {
			v1.push_back(m);
		}
		else {
			v2.push_back(m);
		}
	}
	
	subset["left"] = v1;
	subset["right"] = v2;
	return subset;
}

bool check_all_same(vector< map<int,double> > examples) {
	for(int i=0; i<examples.size(); i++) {
		for(int j=i+1; j<examples.size(); j++) {
			if(examples[i] != examples[j]) {
				return false;
			}
		}
	}
	return true;
}

Node* branch(vector< map<int,double> > examples, float epsilon) {
	vector<int> counts = count_yn(examples);	
	double c = confusion(counts[0],counts[1]);
	int label;
	
	if(c <= epsilon) {
		if(counts[0] > counts[1]) {
			label = 1;
		}
		else {
			label = -1;
		}

		//Node* node = newNode(idx,threshold,label);
		Node* node = newNode(0,0,label);
		return node;
	}
	else {
		int smallest_idx = -1;
		double smallest_threshold = 0;
		double smallest_tc = 1;
	 
		map<int, vector<double> > ts = thresholds(examples);
		map<int, vector<double> > ::iterator iter;
		for(iter = ts.begin(); iter != ts.end(); iter++) {
			int idx = iter->first;
			vector<double> vec = iter->second;
			for(int j=0; j<vec.size(); j++) {
				double t = vec[j];
				vector<int> c = count_yn_by_factor(examples,idx,t);			
				double tc = total_confusion(c[0],c[1],c[2],c[3]);
				
				//if(tc == 0) {
				//	printf("%d, %d, %d, %d \n",c[0],c[1],c[2],c[3]);
				//}
				
				if(min(tc,smallest_tc) == tc) {
					smallest_idx = idx;
					smallest_threshold = t;
					smallest_tc = tc;
				}
			}
		}
	
		//printf("total confusion: %f, index: %d, value: %f \n",smallest_tc,smallest_idx,smallest_threshold);

		map<string, vector< map<int,double> > > sub =  separate_subset(examples,smallest_idx,smallest_threshold);
		vector< map<int,double> > sub1 = sub["left"];
		vector< map<int,double> > sub2 = sub["right"];

		Node* node = newNode(smallest_idx,smallest_threshold,0);
		node->left = branch(sub1,epsilon);
		node->right = branch(sub2,epsilon);
		return node;
	}
}

void print_function(Node* p, int tb){
    if(p->left != NULL || p->right != NULL) {
    	for(int i=0; i<tb; i++) printf("  ");
    	printf("if(attr[%d] > %f) {\n", p->idx, p->threshold);
    	
        print_function(p->left,tb+1);
        
        for(int i=0; i<tb; i++) printf("  ");
        printf("}\n");
        
        for(int i=0; i<tb; i++) printf("  ");
        printf("else {\n");
        
        print_function(p->right,tb+1);
        
        for(int i=0; i<tb; i++) printf("  ");
        printf("}\n");
    }
    else {
    	for(int i=0; i<tb; i++) printf("  ");
    	printf("return %d;\n",p->label);
    }
}

void check_tree(Node* p,int tb){
	for(int i=0; i<tb; i++) { 
        printf("  ");
    }
	printf("%d,%f,%d\n", p->idx, p->threshold, p->label);
	
    if(p->left != NULL || p->right != NULL) {
        check_tree(p->left,tb+1);
        check_tree(p->right,tb+1);
    }
}

/*** Main function ***/

int main(int argc, char *argv[]) {
	
	//string data_file("./heart_scale");
	string data_file(argv[1]);
	
	// float epsilon = 0.1;
	float epsilon = strtof(argv[2], NULL);
	
	load_data(data_file);	
	Node* root = branch(dataset,epsilon);
	
	printf("int tree_predict(double *attr){\n");
	print_function(root,1);
	printf("}\n");
	
	//check_tree(root,0);
    return 0;
}
