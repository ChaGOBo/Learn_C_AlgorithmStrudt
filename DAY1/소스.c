#include<stdio.h>
#include<malloc.h>
/*
��ǥ����(hash + sort)
���׸�Ʈ Ʈ��
offline
------------------------
online Ǯ�̴� dynamic seg tree (���� �𸨴ϴ�..)
*/
#define size 100003 // hash size
#define pnum 29 // �ֳ־����� �𸣰ڱ���;;;; ��� ��� line�Դϴ�.
struct rain
{
	long long val;
};
struct node
{
	int idx;
	struct node* next;
};
struct query
{
	int qk;
	long long a, b;
};
struct query qlist[100010]; // query �����
struct node* f[size], *r[size];
struct rain rtable[400010], sorted[400010]; // struct rain�� �ƴ� long long���� �ϼŵ� �˴ϴ�......
long long* tree; // seg tree
int tidx, n, qn, len;
long long alist[100010], origin[100010];
void input();
unsigned int get_k(long long);
int search(int, long long);
void push(int, int);
void save(long long);
void emq();
struct node pop(int);
void msort(struct rain[], int, int);
void merge(struct rain[], int, int, int);
int bs(struct rain[], int, long long);
void update(int, int);
void doquery(int, int, int, int, int, int);
void q1(long long, long long, int);
void q2(long long, long long, int);
void q3(long long, long long, int, int, int);
void q4(long long, int, int, int);
long long sum(int, int, int, int, int);
int kth(int, int, int, long long);
void make_answer();
int main(void)
{
	make_answer();
	return 0;
}
void make_answer()
{
	int i, len, node, start, end, l, r, g, idx;
	input(); // �Է�
	emq(); // ����Ʈ���� ����ݴϴ�.
	msort(rtable, 0, tidx - 1); //������ ���ݴϴ�.
	/*
	rtable�� �������ݴϴ�.
	rtable�� ����Ǿ��մ� ������ ���� distinct�մϴ�. (set ó�� �ߺ� �ȴ�)
	�׷��� Ž���� ���� �ϱ����ؼ� ������ ���ݴϴ�. (setó�� ���Ĵ��)
	�����˻��� �ϱ� ���ؼ� �Դϴ�.
	*/
	for (len = tidx, node = start = l = r = 1; r - l + 1 < len; l <<= 1, r <<= 1, r |= 1); // seg tree size
	tree = (long long*)calloc(r + 1, sizeof(long long));//seg �Ҵ�
	for (g = l - 1, end = r - l + 1, i = 1; i <= n; i++)
	{
		//�׷� ���� offline���� ������ Ǫ�ϱ� �ʱ��Է��� seg tree�� �ݿ��� ���ݴϴ�.
		/*
		 rtable���� origin[i](���� i�� ������ �׿��ִ� ������)�� �����˻��ؼ� idx�� �޾� ���ϴ�.
		 �׷��� ���� 146 line���� �����Ѵ�� ��ǥ������ ������?

		 origin[i]�� idx�� ġȯ�� ���
		 seg tree leaf node�� ������ ���� origin[i]mm���� ������ ������
		 ���� idxmm ���� ������ ������ ġȯ�� �˴ϴ�.
		*/
		idx = bs(rtable, tidx, origin[i]);
		//�׷��� idx�� �������� seg tree�� update �մϴ�.
		update(g + idx + 1, 1); // seg�� 1base�� �߾��.. rtable�� 0base�̶� + 1 �ؾ� �մϴ�.....
	}
	//���� ������ ������ ���ô�.
	doquery(node, start, end, l, r, g);
	free(tree);//����
	/*
	���� �� Ǯ�� �� ����

	���� hash�� ����� �ʿ䰡 �����ϴ�
	��� �����͸� �����ѵ� �����ϰ� �����˻��� �ϸ�
	�׻� ���� ���� ���ؼ��� �׻� ���� idx�� �ٰ��̱� �����Դϴ�.
	*/
}
void input()
{
	int i, k, s;
	/*
	����;;; ���� C�� ����ϴٺ��� set�� �����ϴ�.
	�׷��� ������ �ؽ��� �����߽��ϴ�. -> ��� ��� �Ǵ� �κ��Դϴ�(���� ����)
	*/
	scanf("%d %d", &n, &qn); // ���ü� ������
	for (i = 1, tidx = 0; i <= n; i++)
	{
		scanf("%lld", &alist[i]); // alist -> area_list�� �����Ͻø� ���ҵ� �մϴ�.
		origin[i] = alist[i];//������������ ������ Ǯ ���̱� ������ origin�迭�� ������ �켱 �صξ�� �մϴ�.
		k = get_k(alist[i]); // hash key�� �޽��ϴ�.
		s = search(k, alist[i]); // key�� value�� ����ؼ� ���� hash table�� value�� ������ �ִ��� �ƴ��� �˻��մϴ�.
		if (s == -1) // ���� hash table�� value�� ���ٸ� ������ �մϴ�.
		{
			push(k, tidx); // chaining hash
			save(alist[i]); // �� �Լ��� ���Ƿ� ������
		}
	}
	//���� ���ø��� �ʱ� ���� �������� ������ �߰� 
	//�������� ������ �ӽô�.
	for (i = 0; i < qn; i++)
	{
		scanf("%d", &qlist[i].qk); // ��������
		if (qlist[i].qk == 4) // 4�� �������?
		{
			//�ϳ� ����
			scanf("%lld", &qlist[i].a);
		}
		else // 1, 2, 3�� ������ 2�� ����
		{
			scanf("%lld %lld", &qlist[i].a, &qlist[i].b);
			if (qlist[i].qk == 1) // 1�� �������?
			{
				//���� qlist[i].a�� ���� qlist[i].bmm���δ�. �Դϴ�.
				alist[qlist[i].a] += qlist[i].b; // area_list[qlist[i].a] += qlist[i].b; �ݿ����ݴϴ�.
				k = get_k(alist[qlist[i].a]);// �׷��� area_list[qlist[i].a]�� �������� key�� �޽��ϴ�.
				s = search(k, alist[qlist[i].a]); // hash table Ž��
				if (s == -1) // ������
				{
					//����
					/*
					PPT�� Ȯ���� �ּ��� �Ф�
					�۷� ������ �ʷ��� �ʹ� ���׿� �ФФФФ�
					*/
					push(k, tidx);
					save(alist[qlist[i].a]);
				}
				/*
				�� �̷��� �ϳ��� 3�� 4�� ���� �����Դϴ�.

				���׸�Ʈ Ʈ���� ��������� ���Ǹ� �̷��� �սô�.
				-> kmm�� ���� ���� ������ ����

				�׷��� mm�� ������ 0 ~ 1000000000000000000(10 ^ 18) �̶� ���׸�Ʈ Ʈ�� ������ ���մϴ�.
				�׷��� �츮�� ��ǥ������ �ؾ��մϴ�.

				ó�� �Է����� ������ n���� ������
				+
				������ ��������� ������ = �������� * 2
				=
				n + qn * 2 = 300000 (�־��϶�)  -> ��������� ����

				�̴� ����� ���׸�Ʈ Ʈ���� �����Ҽ� �ֽ��ϴ�.

				*/
			}
			else if (qlist[i].qk == 2)
			{
				//���� qlist[i].a�� ���� qlist[i].bmm��´�. �Դϴ�.
				alist[qlist[i].a] -= qlist[i].b;
				k = get_k(alist[qlist[i].a]);
				s = search(k, alist[qlist[i].a]);
				if (s == -1)
				{
					push(k, tidx);
					save(alist[qlist[i].a]);
				}
			}
			else
			{
				// 3������ 
				//���� �������� qlist[i].a mm�̻� qlist[i].b mm������ ������ ���� ����.
				k = get_k(qlist[i].a);
				s = search(k, qlist[i].a);
				if (s == -1)
				{
					push(k, tidx);
					save(qlist[i].a);
				}
				k = get_k(qlist[i].b);
				s = search(k, qlist[i].b);
				if (s == -1)
				{
					push(k, tidx);
					save(qlist[i].b);
				}
				//���������� hash table�� Ž���� �� �����մϴ�.
			}
		}
	}
}
unsigned int get_k(long long val)
{
	//�׳� hash_size�� ���� �������� key������ �߽��ϴ�.
	return val % size;
	//hash ���ݴ��Ѵٸ�... �׶� hash func �ٽ� �ٲܰԿ�.....
}
int search(int k, long long val)
{
	//chaining ���� hash ������ �߱⿡ key���� ���� ������ ���� rtable�� ������ ������� Ȯ���մϴ�.
	struct node* temp;
	for (temp = f[k]; temp; temp = temp->next)
	{
		if (rtable[temp->idx].val == val)
		{
			return temp->idx;
		}
	}
	return -1;
}
void push(int k, int idx)
{
	//key�� rtable�� index
	//list push_back ��� ť��� ���ø� �˴ϴ�.
	struct node* temp;
	temp = (struct node*)malloc(sizeof(struct node));
	temp->idx = idx;
	temp->next = 0;
	if (!f[k])
	{
		f[k] = temp;
	}
	else
	{
		r[k]->next = temp;
	}
	r[k] = temp;
}
void save(long long val)
{
	//rtable�̶�� �迭�� �׳� �����ϴ� ������ �Լ��Դϴ�.
	//���⿡�� setó�� �ߺ����� �ʴ� ���ڸ� ������ �˴ϴ�.
	//������ hash table���� �̹� ������� ���� value�� �����ϱ� ��������
	rtable[tidx++].val = val;
}
void emq()
{
	//list ����
	int i;
	for (i = 0; i < size; i++)
	{
		while (f[i])
		{
			pop(i);
		}
	}
}
struct node pop(int k)
{
	//list front pop ��� ť��� ���ŵ� �����մϴ�.
	struct node* temp, ret;
	temp = f[k];
	ret = *f[k];
	f[k] = f[k]->next;
	free(temp);
	return ret;
}
//merge sort�Դϴ�. ����
void msort(struct rain list[], int left, int right)
{
	int mid;
	if (left < right)
	{
		mid = (left + right) >> 1;
		msort(list, left, mid);
		msort(list, mid + 1, right);
		merge(list, left, mid, right);
	}
}
//merge �Դϴ�. ����
void merge(struct rain list[], int left, int mid, int right)
{
	int i, j, k, l;
	k = i = left;
	j = mid + 1;
	while (i <= mid && j <= right)
	{
		if (list[i].val < list[j].val)
		{
			sorted[k++] = list[i++];
		}
		else
		{
			sorted[k++] = list[j++];
		}
	}
	if (mid < i)
	{
		for (l = j; l <= right; l++)
		{
			sorted[k++] = list[l];
		}
	}
	else
	{
		for (l = i; l <= mid; l++)
		{
			sorted[k++] = list[l];
		}
	}
	for (l = left; l <= right; l++)
	{
		list[l] = sorted[l];
	}
}
//���� �˻�. ����
int bs(struct rain list[], int limit, long long val)
{
	int left, mid, right;
	left = 0;
	right = limit - 1;
	while (left <= right)
	{
		mid = (left + right) >> 1;
		if (list[mid].val == val)
		{
			return mid;
		}
		if (list[mid].val < val)
		{
			left = mid + 1;
		}
		else
		{
			right = mid - 1;
		}
	}
}
//seg update�Դϴ�.
void update(int idx, int val)
{
	while (idx)
	{
		tree[idx] += val;
		idx >>= 1;
	}
}
void doquery(int node, int start, int end, int l, int r, int g)
{
	int i;
	for (i = 0; i < qn; i++)
	{
		switch (qlist[i].qk)
		{
		case 1: //1������
			q1(qlist[i].a, qlist[i].b, g);
			break;
		case 2: //2������
			q2(qlist[i].a, qlist[i].b, g);
			break;
		case 3: //3������
			q3(qlist[i].a, qlist[i].b, node, start, end);
			break;
		case 4: //4������
			q4(qlist[i].a, node, start, end);
			break;
		}
	}
}void q1(long long a, long long b, int g) // a�� ������ ���� b��ŭ ���δ�.
{
	int idx;
	//origin[a]�� idx�� ġȯ
	idx = bs(rtable, tidx, origin[a]);
	//seg tree�� idx�� ���� update - > ���� idx��ŭ ���� ������ ������ -1 ��
	update(g + idx + 1, -1);
	//a ������ b��ŭ ���� ���̴� +=b ����
	origin[a] += b;
	//origin[a]�� ���� �ٲ������ �ٽ� idx�� �޾ƿ´�.
	idx = bs(rtable, tidx, origin[a]);
	//���� idx��ŭ ���� ������ ������ +1 ��
	update(g + idx + 1, 1);
}
void q2(long long a, long long b, int g) // a�� ������ ���� b��ŭ ��´�.
{
	int idx;
	// q1�Լ��� ����
	idx = bs(rtable, tidx, origin[a]);
	update(g + idx + 1, -1);
	origin[a] -= b; // ���⸸ �ٲ�ϴ�!!!!!
	idx = bs(rtable, tidx, origin[a]);
	update(g + idx + 1, 1);
}
void q3(long long a, long long b, int node, int start, int end) // ���� amm ~ bmm ��ŭ ���� ������ ����.
{
	int from_idx, to_idx;
	long long ans;
	//a �� from_idx�� ġȯ
	from_idx = bs(rtable, tidx, a);
	//b �� to_idx�� ġȯ
	to_idx = bs(rtable, tidx, b);
	//seg tree leaf node�� ���ǰ� kmm���� ������ �����̴�
	//seg tree���� from_idx ~ to_idx������ ���� ���Ѵ�.
	ans = sum(node, start, end, from_idx + 1, to_idx + 1);
	printf("%lld\n", ans);
}
//seg sum�Լ�
long long sum(int node, int start, int end, int left, int right)
{
	if (left > end || right < start)
	{
		return 0;
	}
	if (left <= start && end <= right)
	{
		return tree[node];
	}
	return sum(node << 1, start, (start + end) >> 1, left, right) + sum((node << 1) | 1, ((start + end) >> 1) + 1, end, left, right);
}
void q4(long long a, int node, int start, int end) //���� �������� a��°�� ���� ���ÿ� ���� ���� �� mm���� ����.
{
	int idx;
	//�������� k��°�� ���� ������ �������� ���ϴ� ����
	/*
	k��°�� ���� -> n - k ��°�� ������ ������!!!
	�׷��� �츮�� 'n - k ��°�� ������ ������'�� ��Ÿ���� seg tree leaf node�� idx�� ���ϸ� �˴ϴ�.
	*/
	idx = kth(node, start, end, n - a + 1);
	/*
	�׷� idx�� �������� �ٽ� ġȯ�ؾ� �մϴ�.
	���ݲ� kmm�� �������� leaf node�� �ε����� ġȯ������

	������ leaf node�� idx�� kmm�� ���������� ġȯ�� ����ϸ� �˴ϴ�.
	*/
	printf("%lld\n", rtable[idx - 1].val); // �̷��Կ�.
}
//segment tree kth func
int kth(int node, int start, int end, long long val)
{
	if (start == end) // idx�� ã��
	{
		return end;
	}
	//�ݹݾ� ����� �����ϸ�
	if (tree[node << 1] >= val) // ó������ �߰� ���� (���� ��) �� ���� val ���� ũ�� �ٽ� ������ �����մϴ�.. �ڵ庸�� �׸��� �׷����ø� ���ذ� �����ϴ�.........
	{
		return kth(node << 1, start, (start + end) >> 1, val);
	}
	return kth((node << 1) | 1, ((start + end) >> 1) + 1, end, val - tree[node << 1]);  // �߰� �������� �� ���� (������ ��)
}