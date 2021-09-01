#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>

#define ARR_SIZE 10

#define RED 0
#define BLACK 1

typedef struct Node
{
	int key;
	void* value;
	int color;
	struct Node* father;
	struct Node* left;
	struct Node* right;
}Node;
Node* NodeCreate(int key, void* value, Node* father);


//插入自平衡操作，参数中的节点颜色必须是红色
//key为刚插入的子节点
void InsertSelfBalancing(Node* node, int key);
//插入节点
Node* NodeInsert(Node* root_node, int key, void* value);


//删除黑节点才需要进行自平衡操作
//key为被删除的子节点
void DeleteSelfBalancing(Node* node, int key);
//把情景2 3转换为情景1
Node* NodeDeleteProcess(Node* delete_node);
//删除节点
Node* NodeDelete(Node* root_node, int key);


//中序遍历
void TraversalCentre(Node* node, char* round);
//左旋
void TurnLeft(Node* node);
//右旋
void TurnRight(Node* node);
//寻找根节点
Node* FindRoot(Node* node);
// 寻找节点
// back_can_insert 为真时，未找到对应的节点则返回可插入的节点
Node* NodeFind(Node* root_node, int key, int back_can_insert);
//寻找一个节点的最左节点
Node* NodeFindLeft(Node* node);
//寻找一个节点的最右节点
Node* NodeFindRight(Node* node);
//节点交换
Node* NodeExchange(Node* delete_node, Node* replace_node);


int main()
{
	Node* root_node = NULL;

	struct timeval start;
	gettimeofday(&start, NULL);//获取当前毫秒
	srand((unsigned int)start.tv_usec);//使用当前毫秒作为随机种子

	int arr[ARR_SIZE], i;
	for(i = 0; i < ARR_SIZE; i++)
		arr[i] = i + 1;
	
	printf("插入顺序：");
	for(i = 0; i < ARR_SIZE; i++)//不重复随机算法
	{
		int rand_sub = rand() % (ARR_SIZE - i);
		root_node = NodeInsert(root_node, arr[rand_sub], NULL);
		printf("%d ", arr[rand_sub]);
		arr[rand_sub] = arr[ARR_SIZE - i - 1];
	}

	// int test_arr[10] = {7, 3, 1, 2, 4, 5, 10, 6, 8, 9}, i;
	// //int test_arr[10] = {2, 5, 9, 1, 4, 8, 6, 10, 7, 3}, i;
	// for (i = 0; i < 10; i++)
	// 	root_node = NodeInsert(root_node, test_arr[i], NULL);
	
	printf("\n");
	TraversalCentre(root_node, "根");//插入后的树

	root_node = NodeDelete(root_node, 5);
	printf("\n");

	TraversalCentre(root_node, "根");//删除后的树

	return 0;
}

Node* NodeCreate(int key, void* value, Node* father)
{
	Node* new_node = (Node*)malloc(sizeof(Node));
	new_node->key = key;
	new_node->value = value;
	new_node->color = RED;		// 默认为红色
	new_node->father = father;
	new_node->left = NULL;
	new_node->right = NULL;
	return new_node;
}

// 寻找节点
// back_can_insert 为真时，未找到对应的节点则返回可插入的节点
Node* NodeFind(Node* root_node, int key, int back_can_insert)
{
	if(key < root_node->key)
	{
		if(root_node->left)
			return NodeFind(root_node->left, key, back_can_insert);
		if(back_can_insert)
			return root_node;
	}
	else if(key > root_node->key)
	{
		if(root_node->right)
			return NodeFind(root_node->right, key, back_can_insert);
		if(back_can_insert)
			return root_node;
	}
	else
		return root_node;

	return NULL;
}

//左旋
void TurnLeft(Node* node)
{
	Node* original_father = node->father;//可能为空
	Node* original_right = node->right;
	Node* original_right_left = node->right->left;//可能为空

	node->father = original_right;
	node->right = original_right_left;

	original_right->father = original_father;
	original_right->left = node;

	if(original_right_left)
		original_right_left->father = node;
	if(original_father)
	{
		if(original_father->left && original_father->left->key == node->key)
			original_father->left = original_right;
		else
			original_father->right = original_right;
	}
}
//右旋
void TurnRight(Node* node)
{
	Node* original_father = node->father;//可能为空
	Node* original_left = node->left;
	Node* original_left_right = node->left->right;//可能为空

	node->father = original_left;
	node->left = original_left_right;

	original_left->father = original_father;
	original_left->right = node;

	if(original_left_right)
		original_left_right->father = node;
	if(original_father)
	{
		if(original_father->left && original_father->left->key == node->key)
			original_father->left = original_left;
		else
			original_father->right = original_left;
	}
}

//插入自平衡操作，参数中的节点颜色必须是红色
//key为刚插入的子节点
void InsertSelfBalancing(Node* node, int key)
{
	//完全不可能发生，如果发生了就是bug
	if(node->color == BLACK)
		return;

	//如果是根节点，直接变成黑色
	if(!node->father)
	{
		node->color = BLACK;
		return;
	}

	//奇怪的双重三目运算符
	Node* brother = !node->father->left ? node->father->left : (
		node->father->left->key == node->key ? node->father->right : node->father->left
	) ;

	//情景4.1：
	if(brother && brother->color == RED)
	{
		node->color = BLACK;
		brother->color = BLACK;
		node->father->color = RED;
		if(node->father->father && node->father->father->color == RED)
			InsertSelfBalancing(node->father->father, node->father->key);
		if(!node->father->father)//PP为根节点时，直接变回黑色
			node->father->color = BLACK;
	}
	//情景4.2
	else if( (!brother || brother->color == BLACK) && node->father->left && node->key == node->father->left->key )
	{
		//情景4.2.1
		if(node->left && node->left->key == key)
		{
			node->color = BLACK;
			node->father->color = RED;
			TurnRight(node->father);
		}
		//情景4.2.2
		else if(node->right && node->right->key == key)
		{
			TurnLeft(node);
			InsertSelfBalancing(node->father, node->key);
		}
	}
	//情景4.3
	else if( (!brother || brother->color == BLACK) && node->father->right && node->key == node->father->right->key )
	{
		//情景4.3.1
		if(node->right && node->right->key == key)
		{
			node->color = BLACK;
			node->father->color = RED;
			TurnLeft(node->father);
		}
		//情景4.3.2
		else if(node->left && node->left->key == key)
		{
			TurnRight(node);
			InsertSelfBalancing(node->father, node->key);
		}
	}
}

//寻找根节点
Node* FindRoot(Node* node)
{
	if(node && node->father)
	{
		node = node->father;
		return FindRoot(node);
	}
	return node;
}

//插入节点
Node* NodeInsert(Node* root_node, int key, void* value)
{
	//情景1：红黑树为空树
	if(!root_node)
	{
		root_node = NodeCreate(key, value, NULL);
		root_node->color = BLACK;
		return root_node;
	}

	Node* now_node = NodeFind(root_node, key, 1);

	//情景2：节点已存在
	if(now_node->key == key)
	{
		now_node->value = value;
		return FindRoot(root_node);
	}

	Node* father_node = now_node;
	Node* new_node = NodeCreate(key, value, father_node);

	//管他那么多，先插入
	if(father_node->key < key)
		father_node->right = new_node;
	else
		father_node->left = new_node;

	//情景3:父节点为黑色
	if(father_node->color == BLACK)
		return FindRoot(root_node);

	//情景4：父节点为红色
	if(father_node->color == RED)
		InsertSelfBalancing(father_node, key);
	
	return FindRoot(root_node);
}

//删除黑节点才需要进行自平衡操作
//key为被删除的子节点
void DeleteSelfBalancing(Node* node, int key)
{
	//情景2.1
	if(node->left && node->left->key == key && node->right)//黑色叶子节点一定有兄弟？
	{
		//情景2.1.1
		if(node->right->color == RED)
		{
			node->right->color = BLACK;
			node->color = RED;
			TurnLeft(node);
			DeleteSelfBalancing(node, key);
		}
		//情景2.1.2
		if(node->right->color == BLACK)
		{
			//情景2.1.2.1
			if(node->right->right && node->right->right->color == RED)
			{
				node->right->color = node->color;
				node->color = BLACK;
				node->right->right->color = BLACK;
				TurnLeft(node);
			}
			//情景2.1.2.2
			else if( (!node->right->right || node->right->right->color == BLACK) && 
				( node->right->left  && node->right->left->color  == RED) )
			{
				node->right->color = RED;
				node->right->left->color = BLACK;
				TurnRight(node->right);
				DeleteSelfBalancing(node, key);
			}
			//情景2.1.2.3
			else if(	(!node->right->left  || node->right->left->color  == BLACK) &&
				(!node->right->right || node->right->right->color == BLACK) )
			{
				node->right->color = RED;
				DeleteSelfBalancing(node->father, node->key);
			}
		}
	}
	//情景2.2
	else if(node->right && node->right->key == key && node->left)//黑色叶子节点一定有兄弟？
	{
		//情景2.2.1
		if(node->left->color == RED)
		{
			node->left->color = BLACK;
			node->color = RED;
			TurnRight(node);
			DeleteSelfBalancing(node, key);
		}
		//情景2.2.2
		if(node->left->color == BLACK)
		{
			//情景2.2.2.1
			if(node->left->left && node->left->left->color == RED)
			{
				node->left->color = node->color;
				node->color = BLACK;
				node->left->left->color = BLACK;
				TurnRight(node);
			}
			//情景2.2.2.2
			if( (!node->left->left  || node->left->left->color  == BLACK) && 
				( node->left->right && node->left->right->color == RED) )
			{
				node->left->color = RED;
				node->left->right->color = BLACK;
				TurnLeft(node->left);
				DeleteSelfBalancing(node, key);
			}
			//情景2.2.2.3
			if(	(!node->left->right  || node->left->right->color  == BLACK) &&
				(!node->left->left || node->left->left->color == BLACK) )
			{
				node->left->color = RED;
				DeleteSelfBalancing(node->father, node->key);
			}
		}
	}
}

//寻找一个节点的最左节点
Node* NodeFindLeft(Node* node)
{
	if(node->left)
		return NodeFindLeft(node->left);
	return node;
}

//寻找一个节点的最右节点
Node* NodeFindRight(Node* node)
{
	if(node->right)
		return NodeFindRight(node->right);
	return node;
}

//节点交换
Node* NodeExchange(Node* delete_node, Node* replace_node)
{
	//把删除节点的父节点的子节点改为替换节点
	if( delete_node->father && delete_node->father->left && 
		delete_node->father->left->key == delete_node->key )
		delete_node->father->left = replace_node;
	else if(delete_node->father)
		delete_node->father->right = replace_node;
	
	//把删除节点的子节点的父节点改为替换节点（如果删除节点的子节点是替换节点则跳过）
	if( delete_node->left && delete_node->left->key != replace_node->key)
		delete_node->left->father = replace_node;
	if( delete_node->right && delete_node->right->key != replace_node->key)
		delete_node->right->father = replace_node;
	
	//把替换节点的颜色和父子节点改为删除节点的颜色和父子节点（如果删除节点的子节点是替换节点则设为空）
	replace_node->color = delete_node->color;
	replace_node->father = delete_node->father;
	replace_node->left = delete_node->left ? 
		(delete_node->left->key == replace_node->key ? NULL : delete_node->left) : 
		NULL;
	replace_node->right = delete_node->right ? 
		(delete_node->right->key == replace_node->key ? NULL : delete_node->right) : 
		NULL;
	
	return delete_node;
}

//把情景2 3转换为情景1
Node* NodeDeleteProcess(Node* delete_node)
{
	//情景1：不存在子节点
	if(delete_node->father && !delete_node->left && !delete_node->right)
	{
		if(delete_node->color == BLACK)
			DeleteSelfBalancing(delete_node->father, delete_node->key);

		//把删除节点(替换节点)的父节点对应的子节点改为空
		if( delete_node->father->left && delete_node->father->left->key == delete_node->key)
			delete_node->father->left = NULL;
		else
			delete_node->father->right = NULL;
		return delete_node;
	}

	//情景2：只存在一个子节点
	if( (delete_node->left && !delete_node->right) || (!delete_node->left && delete_node->right) )
	{
		Node* replace_node = delete_node->left ? delete_node->left : delete_node->right;
		replace_node = NodeDeleteProcess(replace_node);
		return NodeExchange(delete_node, replace_node);
	}

	//情景3：存在两个子节点
	if(delete_node->left && delete_node->right)
	{
		Node* replace_node = NodeFindLeft(delete_node->right);//右字节点的最左节点
		replace_node =  NodeDeleteProcess(replace_node);
		return NodeExchange(delete_node, replace_node);
	}

	return delete_node;
}

//删除节点
Node* NodeDelete(Node* root_node, int key)
{
	Node* delete_node = NodeFind(root_node, key, 0);

	//节点不存在
	if(!delete_node)
		return NULL;
	
	Node* backup_node = root_node->left ? root_node->left : backup_node->right;
	Node* delete_node2 = NodeDeleteProcess(delete_node);

	if(delete_node->key != delete_node2->key)
	{
		printf("error");
		return NULL;
	}

	root_node = root_node->key == key ? FindRoot(backup_node) : FindRoot(root_node);
	free(delete_node);
	return root_node;
}

//中序遍历
void TraversalCentre(Node* node, char* round)
{
	if(node)
	{
		TraversalCentre(node->left, "左");
		printf("\tdata:%d, round:%s, color:%s, left:%d, right:%d\n", node->key, round, 
			node->color == RED ? "红" : "黑", node->left ? node->left->key : -1,
			node->right ? node->right->key : -1);
		TraversalCentre(node->right, "右");
	}
}
