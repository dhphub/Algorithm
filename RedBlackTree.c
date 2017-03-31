/* 红黑树的实现
 * 红黑树是带有平衡条件(着色性质)的二叉查找树
 * 1)节点要么红色要么黑色
 * 2)根节点为黑色
 * 3)叶节点(哨兵)必黑
 * 4)若节点为红色，其子节点必黑
 * 5)一个节点到一个NULL指针的路径必须包含相同的黑色节点
 * 2016-12-24
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 红黑树的类型声明 */
typedef int KeyType;
typedef enum {Red , Black} ColorType;

/* 红黑树节点类型 */
typedef struct TreeNode{
	KeyType Key;
	struct TreeNode *Left;
	struct TreeNode *Right;
	struct TreeNode *Parent;
	ColorType Color;
}Node,*PNode;

/* 红黑树树类型 */
typedef struct RBTree{
	PNode Root;
	PNode Nil;    
}RBTree,*PRBTree;

/* 构造函数 */
PRBTree Intialize();
void MakeEmpty(PRBTree T);
PNode Search(KeyType k,PRBTree T);
void Insert(KeyType k,PRBTree T);
void InsertFixup(PRBTree T,PNode Z);
void DeleteWithKey(KeyType k,PRBTree T);
void DeleteFixup(PRBTree T,PNode Z);
void TransPlant(PRBTree,PNode u,PNode v);
PNode FindMin(PRBTree T,PNode t);
PNode FindMax(PRBTree T,PNode t);
void LeftRotate(PRBTree T,PNode X);
void RightRotate(PRBTree T,PNode X);
void MidOutputTree(PRBTree T,PNode x);
void PreOutputTree(PRBTree T,PNode x);
void BehOutputTree(PRBTree T,PNode x);
void menu();
char *StringColor(ColorType col);

/* 转化输出 */
char *StringColor(ColorType col){
	switch(col){
		case Red:
			return "红";
			break;
		case Black:
			return "黑";
			break;
		default:
			break;
	}
	return "错误";
}
void menu()
{
    printf("*---------------------红黑树--------------------*\n");
    printf("1---------创建红黑树       2-----------置空红黑树\n");
    printf("3---------插入节点         4-----------删除节点\n");
    printf("5---------最小节点         6-----------最大节点\n");
	printf("7---------查找节点         8-----------输出红黑树\n");
    printf("0---------帮助             e-----------退出\n");
    printf("*---------------------------------------------*\n");
}

/* 中序输出 */
void MidOutputTree(PRBTree T,PNode x){
	if(x != T -> Nil){
	MidOutputTree(T,x -> Left);
	printf("%d\t",x -> Key);
	MidOutputTree(T,x -> Right);
	}
}
void PreOutputTree(PRBTree T,PNode x){
	if(x != T->Nil){
		printf("%d\t",x->Key);
		PreOutputTree(T,x->Left);
		PreOutputTree(T,x->Right);
	}
}
void BehOutputTree(PRBTree T,PNode x){
	if(x != T->Nil){
		BehOutputTree(T,x->Left);
		BehOutputTree(T,x->Right);
		printf("%d\t",x->Key);
	}
}
/* 初始化创建红黑树 */
PRBTree Intialize(){
	PRBTree T;
	KeyType k;
	T = malloc(sizeof(RBTree));
	T -> Nil = malloc(sizeof(Node));
	T -> Root = malloc(sizeof(Node));
	T -> Nil -> Key = -1;
	T -> Nil -> Color = Black;
	T -> Nil -> Left = T -> Nil -> Right = T -> Nil -> Parent = NULL;
	T -> Root = T -> Nil;
	printf("输入节点的值(输入-1则结束):");
	scanf("%d",&k);
	while(k!=-1){
		Insert(k,T);
		scanf("%d",&k);
	}
	return T;
}

/* 按关键字查找 */
PNode Search(KeyType k,PRBTree T){
	PNode x = T -> Root;
	while(x -> Key != k && x != T -> Nil){
		if(k < x -> Key)
			x = x -> Left;
		else
			x = x -> Right;
	}
	return x;
}

/* 置空 */
void MakeEmpty(PRBTree T){
	T -> Root = T -> Nil;
}

/* 最大值非递归实现 */
PNode FindMax(PRBTree T,PNode t){
	PNode x = t;
	while(x -> Right != T -> Nil)
		x = x -> Right;
		return x;
}

/* 最小值非递归实现 */
PNode FindMin(PRBTree T,PNode t){
	PNode x = t;
	while(x -> Left != T -> Nil)
		x = x -> Left;
	return x;
}

/* 左旋 */
void LeftRotate(PRBTree T,PNode X){
	PNode Y;
	Y = X -> Right;
	X -> Right = Y -> Left;
	if(Y -> Left != T -> Nil)
		Y -> Left -> Parent = X;
	Y -> Parent = X -> Parent;
	if(X -> Parent == T -> Nil)
		T -> Root = Y;
	else if(X == X -> Parent -> Left)
		X -> Parent -> Left = Y;
	else
		X -> Parent -> Right = Y;
	Y -> Left = X;
	X -> Parent = Y;
}

/* 右旋 */
void RightRotate(PRBTree T,PNode X){
	PNode Y;
	Y = X -> Left;
	X -> Left = Y -> Right;
	if(Y -> Right != T -> Nil)
		Y -> Right -> Parent = X;
	Y -> Parent = X -> Parent;
	if(X -> Parent == T -> Nil)
		T -> Root = Y;
	else if(X == X -> Parent -> Left)
		X -> Parent -> Left = Y;
	else
		X -> Parent -> Right = Y;
	Y -> Right = X;
	X -> Parent = Y;
}

/* 插入 */
void Insert(KeyType k,PRBTree T){
	PNode X,Y,Z;
	Y = T -> Nil;
	X = T -> Root;
	Z = malloc(sizeof(Node));
	Z -> Key = k;
	while(X != T -> Nil){
		Y = X;
		if(Z -> Key < X -> Key)
			X = X -> Left;
		else if(Z -> Key > X -> Key)
			X = X -> Right;
		else{
			printf("已经存在该节点!\n");
			return;
		}
	}
	Z -> Parent = Y;
	if(Y == T -> Nil)
		T -> Root = Z;
	else if(Z -> Key < Y -> Key)
		Y -> Left = Z;
	else
		Y -> Right = Z;
	Z -> Left = Z -> Right = T -> Nil;
	Z -> Color = Red;
	InsertFixup(T,Z);
	printf("插入成功!\n");
}

/* 插入后修复红黑树性质 */
void InsertFixup(PRBTree T,PNode Z){
	PNode Y;
	while(Z -> Parent -> Color == Red){
		if(Z -> Parent == Z -> Parent -> Parent -> Left){
			Y = Z -> Parent -> Parent -> Right;
			if(Y -> Color == Red){
				Z -> Parent -> Color = Black;
				Y -> Color = Black;
				Z -> Parent -> Parent -> Color = Red;
				Z = Z -> Parent -> Parent;
			}
			else{ 
				if(Z == Z -> Parent -> Right){
					Z = Z -> Parent;
					LeftRotate(T,Z);
				}
				Z -> Parent -> Color = Black;
				Z -> Parent -> Parent -> Color = Red;
				RightRotate(T,Z -> Parent -> Parent);
			}
		}
		else{
			Y = Z -> Parent -> Parent -> Left;
			if(Y -> Color == Red){
				Z -> Parent -> Color = Black;
				Y -> Color = Black;
				Z -> Parent -> Parent -> Color = Red;
				Z = Z -> Parent -> Parent;
				//LeftRotate(T,Z);
			}
			else{
				if(Z == Z -> Parent -> Left){
					Z = Z -> Parent;
					RightRotate(T,Z);
				}
				Z -> Parent -> Color = Black;
				Z -> Parent -> Parent -> Color = Red;
				LeftRotate(T,Z -> Parent -> Parent);
			}
		}
	}
	T -> Root -> Color = Black;
}

/* 删除辅助函数,用v代替u */
void TransPlant(PRBTree T,PNode u,PNode v){
	if(u -> Parent == T -> Nil)
		T -> Root = v;
	else if(u == u -> Parent -> Left)
		u -> Parent -> Left = v;
	else
		u -> Parent -> Right = v;
	v -> Parent = u -> Parent;
}

/* 删除函数 */
void DeleteWithKey(KeyType k,PRBTree T){
	PNode x,y,z;
	ColorType y_original_color;
	z = Search(k,T);
	y = z;
	y_original_color = y -> Color;
	if(z -> Left == T -> Nil){
		x = z -> Right;
		TransPlant(T,z,z -> Right);
	}
	else if(z -> Right == T -> Nil){
		x = z -> Left;
		TransPlant(T,z,z -> Left);
	}
	else{
		y = FindMin(T,z -> Right);
		y_original_color = y ->  Color;
		x = y -> Right;
		if(y -> Parent == z)
			x -> Parent = y;
		else{
			TransPlant(T,y,y -> Right);
			y -> Right = z -> Right;
			y -> Right -> Parent = y;
		}
		TransPlant(T,z,y);
		y -> Left = z -> Left;
		y -> Left -> Parent = y;
		y -> Color = z -> Color;
	}
	if(y_original_color == Black)
		DeleteFixup(T,x);
}

/* 删除修复函数保持红黑树的性质 */
void DeleteFixup(PRBTree T,PNode x){
	PNode w;
	while(x != T -> Root && x -> Color == Black){
		if(x == x -> Parent -> Left){
			w = x -> Parent -> Right;
			if(w -> Color == Red){
				w -> Color = Black;
				x -> Parent -> Color = Red;
				LeftRotate(T,x -> Parent);
				w = x -> Parent -> Right;
			}
			if(w -> Left -> Color == Black && w -> Right -> Color == Black){
				w -> Color = Red;
				x = x -> Parent;
			}
			else{ 
				if(w -> Right -> Color == Black){
				w -> Left -> Color = Black;
				w -> Color = Red;
				RightRotate(T,w);
				w = x -> Parent -> Right;
				}
				w -> Color = x -> Parent -> Color;
				x -> Parent -> Color = Black;
				w -> Right -> Color = Black;
				LeftRotate(T,x -> Parent);
				x = T -> Root;
			}
		}
		else{
			w = x -> Parent -> Left;
			if(w -> Color == Red){
				w -> Color = Black;
				x -> Parent -> Color = Red;
				RightRotate(T,x -> Parent);
				w = x -> Parent -> Left;
			}
			if(w -> Right -> Color == Black && w -> Left -> Color == Black){
				w -> Color = Red;
				x = x -> Parent;
			}
			else{
				if(w -> Left -> Color == Black){
					w -> Right -> Color = Black;
					w -> Color = Red;
					LeftRotate(T,w);
					w = x -> Parent -> Left;
				}
				w -> Color = x -> Parent -> Color;
				x -> Parent -> Color = Black;
				w -> Left -> Color = Black;
				RightRotate(T,x -> Parent);
				x = T -> Root;
			}
		}
	}
	x -> Color = Black;
}

int main()
{
    PRBTree Tree = NULL;
    KeyType key;
    PNode x;
    char ch;
    menu();
    while (1)
    {
        printf(">>");
        ch = getchar();
        switch (ch){
			case '0':
				menu();
				break;
			case '2':
				if(Tree == NULL||Tree->Root==Tree->Nil){
					printf("空树,请先创建!\n");
					break;
				}
				MakeEmpty(Tree);
				printf("已经置空\n");
				break;
			case '1':
				Tree = Intialize();
				break;
			case '3':
				printf("输入关键字:");
				scanf("%d", &key);
				Insert(key,Tree);
				//printf("插入成功!\n");
				break;
			case '4':
				if (Tree == NULL||Tree->Root==Tree->Nil){
					printf("这是一颗空树无法删除!\n");
					break;
				}
				printf("输入想要删除节点的关键字:");
				scanf("%d", &key);
				x = Search(key, Tree);
				if (x != Tree->Nil){
					DeleteWithKey(key, Tree);
					printf("删除成功!\n");
				}
				else 
					printf("没有节点与此关键字匹配! \n");
				break;
			case '5':
				if (Tree == NULL || Tree->Root==Tree->Nil){
					printf("空树，请先创建!\n");
					break;
				}
				x = FindMin(Tree,Tree -> Root);
				if(x == Tree -> Nil)
					printf("没有此节点\n");
				else
					printf("最小节点为:%d\n其颜色为:%s\n",x->Key,StringColor(x->Color));
				if(x -> Parent != Tree -> Nil)
					printf("父节点为:%d\n",x->Parent->Key);
				else
					printf("无父节点!\n");
				if(x -> Left != Tree -> Nil)
					printf("左孩子为:%d\n",x->Left->Key);
				else
					printf("无左孩子!\n");
				if(x -> Right != Tree -> Nil)
					printf("右孩子为:%d\n",x->Right->Key);
				else
					printf("无右孩子!\n");
				break;
				break;
			case '6':
				if (Tree == NULL || Tree->Root==Tree->Nil){
					printf("树为空请先创建!\n");
					break;
				}
				x = FindMax(Tree, Tree->Root);
				if(x == Tree -> Nil)
					printf("没有此节点\n");
				else
					printf("最大节点为:%d\n其颜色为:%s\n",x->Key,StringColor(x->Color));
				if(x -> Parent != Tree -> Nil)
					printf("父节点为:%d\n",x->Parent->Key);
				else
					printf("无父节点!\n");
				if(x -> Left != Tree -> Nil)
					printf("左孩子为:%d\n",x->Left->Key);
				else
					printf("无左孩子!\n");
				if(x -> Right != Tree -> Nil)
					printf("右孩子为:%d\n",x->Right->Key);
				else
					printf("无右孩子!\n");
				break;
				break;
			case '7':
				if(Tree == NULL||Tree->Root==Tree->Nil){
					printf("树为空,请先创建\n!");
					break;
				}
				printf("输入查找节点的值:");
				scanf("%d",&key);
				x = Search(key,Tree);
				if(x == Tree -> Nil)
					printf("没有此节点\n");
				else{
					printf("此节点存在!\n其颜色为:%s\n",StringColor(x->Color));
					if(x -> Parent != Tree -> Nil)
						printf("父节点为:%d\n",x->Parent->Key);
					else
						printf("无父节点!\n");
					if(x -> Left != Tree -> Nil)
						printf("左孩子为:%d\n",x->Left->Key);
					else
						printf("无左孩子!\n");
					if(x -> Right != Tree -> Nil)
						printf("右孩子为:%d\n",x->Right->Key);
					else
						printf("无右孩子!\n");
				}
				break;
			case '8':
				if (Tree == NULL || Tree->Root==Tree->Nil){
					printf("树为空,请先创建!\n");
					break;
				}
				printf("中序:"); MidOutputTree(Tree,Tree->Root); printf("\n");
			    printf("前序:"); PreOutputTree(Tree,Tree->Root); printf("\n");
				printf("后序:"); BehOutputTree(Tree,Tree->Root); printf("\n");
				break;
			case 'e':
				return 0;
			case '\n':
				break;
			default:
				menu();
				break;
        }
    }
    return 0;
}
