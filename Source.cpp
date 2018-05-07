#include <iostream>
#include<fstream>

#include "winbgim.h"                 
#include <Windows.h> 

#include<vector>
using namespace std;

int window_ID=-1;
int latest_insert;

struct Node{
	int *keys;
	Node **pointers;
	
	int t;
	int size;
	int total_size;

	bool isRoot;
	bool isLeaf;

	Node(){
		keys = nullptr;
		pointers = nullptr;
	}

	Node(int _t){
		t = _t;
		total_size=(2*t)-1;
		keys = new int[total_size];
		pointers=new Node*[2*t];
		for(int i=0;i<2*t;i++)
			pointers[i]=nullptr;

		size = 0;
		
		isRoot=false;
		isLeaf=false;
	}	

	Node* get_Left_Sibling(Node *parent){
		if(parent==nullptr || parent->pointers[0]==this)
			return nullptr;

		for(int i=1;i<this->size;i++)
			if(parent->pointers[i]==this)
				return parent->pointers[i-1];
	}

	Node* get_Right_Sibling(Node *parent){
		if(parent==nullptr || parent->pointers[parent->size]==this)
			return nullptr;

		for(int i=parent->size-1;i>=0;i--)
			if(parent->pointers[i]==this)
				return parent->pointers[i+1];
	}

	int getPosition_in_Parent(Node *parent){
		
		for(int i=0;i<this->size;i++)
			if(parent->pointers[i]==this)
				return i;
	}
};

class B_Tree{

	int t;
	Node *root;

public:
	B_Tree(){
		root = nullptr;
	}

	B_Tree(int _t){
		t=_t;
		root = new Node(t);
		root->isRoot=true;
		root->isLeaf=true;
	}

	void Insert(int x){

		int i,j,key;
		Node *curr=root,*parent;

		parent=nullptr;
		while(true){
			if(curr->size==curr->total_size){				//If a full node is found in the path

				if(curr->isRoot){                           //If it's root
					Node *new1=new Node(t);
					Node *new2=new Node(t);
					
					if(curr->isLeaf){
						new1->isLeaf=true;
						new2->isLeaf=true;
					}
					for(i=0;i<t-1;i++)
						new1->keys[i]=curr->keys[i];
					new1->size=i;

					if(!curr->isLeaf){
						for(i=0;i<t-1;i++){
							new1->pointers[i]=curr->pointers[i];
						}
						new1->pointers[i]=curr->pointers[i];
					}

					for(i=0;i<t-1;i++)
						new2->keys[i]=curr->keys[i+t];
					new2->size=i;

					if(!curr->isLeaf){
						for(i=0;i<t-1;i++){
							new2->pointers[i]=curr->pointers[i+t];
						}
						new2->pointers[i]=curr->pointers[i+t];
					}
					
					root->keys[0]=root->keys[t-1];
					root->size=1;
					root->isLeaf=false;
					root->pointers[0]=new1;
					root->pointers[1]=new2;

					curr=root;
				}
				else{  

					Node *new1=new Node(t);
					if (curr->isLeaf)
						new1->isLeaf=true;
					
					for(i=0;i<t-1;i++)
						new1->keys[i]=curr->keys[i];
					new1->size=i;

					key=curr->keys[t-1];
					for(i=t,j=0;i<(2*t)-1;i++,j++)
						curr->keys[j]=curr->keys[i];
					curr->size=j;

					if (!curr->isLeaf){
						for(i=0;i<t-1;i++){
							new1->pointers[i]=curr->pointers[i];
						}
						new1->pointers[i]=curr->pointers[i];

						for(i=t,j=0;i<(2*t)-1;i++,j++){
							curr->pointers[j]=curr->pointers[i];

						}
						curr->pointers[j]=curr->pointers[i];

					}

					for(i=parent->size-1;i>=0 && parent->keys[i]>=key;i--){
						parent->keys[i+1]=parent->keys[i];
						parent->pointers[i+2]=parent->pointers[i+1];
					}
					parent->keys[i+1]=key;
					parent->size++;

				
					parent->pointers[i+1]=new1;
					parent->pointers[i+2]=curr;

					curr=parent;
				}
				
			}
			else if(curr->isLeaf){

				int i;
				for(i=curr->size-1;i>=0&&curr->keys[i]>=x;i--)
					curr->keys[i+1]=curr->keys[i];

				curr->keys[i+1]=x;
				curr->size++;

				latest_insert=x;
				return;
			}
			else{

				for(i=0;i<curr->size && curr->keys[i]<=x;i++)
					;

				parent=curr;
				curr=curr->pointers[i];
					
			}
		}
	}

	void Delete(int x){

		
		int i,j,key;
		Node *curr=root,*parent;
		parent=nullptr;
		while(curr!=nullptr){

			if(curr->size<=t-1 && !curr->isRoot){			//A node is found with min. elements

				Node *left_sibling=curr->get_Left_Sibling(parent);
				Node *right_sibling=curr->get_Right_Sibling(parent);

				if(curr->isRoot){
					;
				}

				if(left_sibling!=nullptr && left_sibling->size>(t-1)){            //Borrowing from left sibling

					for(int i=curr->size;i>=0;i++)				//Creating a space at the start of the current node for the key from parent to be placed
						curr->keys[i]=curr->keys[i-1];
					
					curr->keys[0]=parent->keys[curr->getPosition_in_Parent(parent)-1];
					parent->keys[curr->getPosition_in_Parent(parent)-1]=left_sibling->keys[left_sibling->size-1];

					left_sibling->size--;
					curr->size++;

				}
				else if(right_sibling!=nullptr && right_sibling->size>(t-1)){          //Borrowing from right sibling
					
					curr->keys[curr->size]=parent->keys[curr->getPosition_in_Parent(parent)];
					parent->keys[curr->getPosition_in_Parent(parent)]=right_sibling->keys[0];

					right_sibling->size--;
					curr->size++;
				}
				else if(right_sibling!=nullptr){                         //Merging with right sibling

					curr->keys[curr->size]=parent->keys[curr->getPosition_in_Parent(parent)];		//Copying the respective parent key into the merged node

					for(int i=0,j=t;i<curr->size;i++,j++)				//Copying the current node's data into the right sibling
						curr->keys[j]=right_sibling->keys[i];

					curr->size=(2*t)-1;							//Updating the size of the mergde node

					for(int i=curr->getPosition_in_Parent(parent);i<parent->size-1;i++){			//Shifting the keys and pointers in the parent to the right
						parent->keys[i]=parent->keys[i+1];
						parent->pointers[i+1]=parent->pointers[i+2];
					}
					parent->pointers[i+1]=nullptr;
					parent->size--;										//Updating the size of parent Node

					delete right_sibling;

					curr=parent;
				}
				else if(left_sibling!=nullptr){                         //Merging with left sibling

					left_sibling->keys[left_sibling->size]=parent->keys[curr->getPosition_in_Parent(parent)];		//Copying the respective parent key into the merged node

					for(int i=0,j=t;i<curr->size;i++,j++)				//Copying the current node's data into the right sibling
						left_sibling->keys[j]=curr->keys[i];

					left_sibling->size=(2*t)-1;							//Updating the size of the mergde node

					for(int i=curr->getPosition_in_Parent(parent)-1;i<parent->size-1;i++){			//Shifting the keys and pointers in the parent to the right
						parent->keys[i]=parent->keys[i+1];
						parent->pointers[i+1]=parent->pointers[i+2];
					}

					parent->size--;										//Updating the size of parent Node

					delete curr;

					curr=parent;
				}
			}
			else{

				for(i=0;i<curr->size;i++){
					if(curr->keys[i]==x){
						for(int j=i;j<curr->size;j++)
							curr->keys[j]=curr->keys[j+1];
						curr->size--;
						cout<<x<<" deleted\n\n";
						return;
					}
					else if(x<curr->keys[i])
						break;
				}
				parent=curr;
				curr=curr->pointers[i];				//When i==curr->size

			}

		}

	}

	bool Search(int key){

		Node *curr=root;
		int i;
		while(curr!=nullptr){

			for(i=0;i<curr->size;i++){
				if(key==curr->keys[i])
					return true;
				else if(key<curr->keys[i]){
					curr=curr->pointers[i];
					break;
				}
			}
			curr=curr->pointers[i];				//When i==curr->size
		}
		return false;
	}	

	void Print(){

		if(root->size!=0){                                 //Tree not empty
			if(window_ID!=-1)
				closegraph(window_ID);
			window_ID=initwindow(1000, 600, "B-Tree");

			int row=100;
			int col=0;

			vector <Node*> Queue;
			Node *curr;
			int count,limit,next_limit;
			char arr[15];

			Queue.push_back(root);
			count=0;limit=1;next_limit=0;
			while(!Queue.empty()){
				curr=Queue.at(0);
				Queue.erase(Queue.begin());
				
				setcolor(12);
				rectangle(col+20,row-5,col+(33*curr->size)+10,row+20);				//L,T,R,B
				setcolor(15);
				for(int i=0;i<curr->size;i++){
					_itoa_s(curr->keys[i],arr,10);
					if(curr->keys[i]==latest_insert)
						setcolor(14);
					else 
						setcolor(11);
					outtextxy(col+=25,row,arr);
				}

				col+=20;

				if(!curr->isLeaf){
					for(int i=0;i<curr->size+1;i++)
						Queue.push_back(curr->pointers[i]);

				}
				next_limit+=curr->size+1;
				if(++count==limit){
					count=0;
					limit=next_limit;
					next_limit=0;
					row+=35;
					col=0;
				}
			}
		}
	}
	
};

int main(){

	B_Tree obj(3);

	ifstream fin("Input.txt");
	int a,choice;	

	while(true){
		system("cls");
		cout<<"What do you want to do?\n\t1) Insert\n\t2) Delete\n\t3) Search\n\t4) Print\n\n";
		cout<<"Enter the no. of action: ";
		cin>>choice;
		switch(choice){

		case 1:
			cout<<"Enter value to insert: ";
			cin>>a;
			obj.Insert(a);
			break;
			
		case 2:
			cout<<"Enter value to delete: ";
			cin>>a;
			obj.Delete(a);
			break;

		case 3:
			obj.Search(a);
			break;

		case 4:
			obj.Print();
			break;
		}

	}

	system("pause");
	return 0;
}