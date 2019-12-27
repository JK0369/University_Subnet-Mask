#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ���α׷��� �ռ� scanf�� ����ϱ� ������ ��ó���⿡ _CRT_SECURE_NO_WARNING �߰��Ѵ�.
// subnet_calculator
// 201352048 ������

/* source program ����

1. ����׿� �Լ�
1) get_size : bit���� ���̸� �˾Ƴ��� �Լ�
2) print_int_to_bit : integer���� bit�� ����ϴ� �Լ�

2. ���� ���� ���� (Ű���� �Է¿� ���� ����� ���� ����)

3. ���� ó�� �Լ� (���� �޼��� ���)
1) 4���� ��Ʈ��ũ ip�� ���޴� ���
2) ��Ʈ��ũ ip �ּ� ���� 0~255���̰� �ƴ� ���
3) ���ڰ��ƴ� ���ڸ� �Է��� ���
4) 127�� �����ϴ� ip�� �Է��� ���
5) ��Ʈ��ũ ID�� �߸� �Է��� ���(subnet mask�� ��)
6) �� ����� �� ȣ��Ʈ�� ���� ��� �Է����� �ʾҰų� �ʰ��Ͽ� �Է��� ���

3. �������� ���� �Լ�

4. Ű���� �Է� ���� �Լ�
1) my_split : �Ű��������� ���� ���ڿ� ip �ּҸ� split�Ͽ� ������ �ٲٴ� �Լ�
2) in : Ű���� �Է��� �״�� �޾Ƽ� ������ ���� (�� �Լ����� my_slpitȣ��, �������� ���ı��� �Ϸ�)

5. subnetting ���� �Լ�
1) search_min : (host ũ�� + 2) ���� ū 2�� ������ �� �߿���, �ּ� ���� �� ������������ ��ȯ�ϴ� �Լ� ȣ��
2) store : �� subnet�� �ʿ��� host��(2�� n��) �����ϴ� �Լ�(���������� host_requirement�� ����)
3) convert_to_32bit : ���ݰ��� �޾Ƽ� 32bit������ ��ȯ�ϴ� �Լ�
4) execute : subnetting ����(��ȯ���� ù ��° subnet�� ip min ��)
5) get_min_max : ù ��° ������� �Ҵ簡���� �ּҰ� ip�� �޾Ƽ�, ������ ������� min,max���� ���ϴ� �Լ�
6) convert_to_octet : 10������ �Է¹޾�, 4���� ip �迭(����)�� ��ȯ�Ͽ� ��ȯ�ϴ� �Լ�
7) convert : �� subnet�� min�� max���� ���� ������ 2���� �迭 ��ȯ (���⼭ convert_to_octetȣ��)
��ȯ ��[i][j] : i=min, max, min, max,,, �ݺ�(�Ҵ�Ұ��� NULL), j=���ݰ��� �ε���
8) print_reult : ���� ���� ����ϴ� �Լ�

*/


// *For test //
// bit���� ���̸� �˾Ƴ��� �Լ� 
int get_size(long long int value) {
	int i = 0;
	while (value != 0 && value != -1) {
		i++;
		value = value >> 1;
	}
	return i;
}

// *For test //
// integer���� bit�� ����ϴ� �Լ�
void print_int_to_bit(long long int value) {

	if (value == -1) {
		printf("%lld = unused\n", value);
		return;
	}
	// bit ����
	int size = get_size(value);

	printf("%lld = ", value);
	for (int i = size - 1; i >= 0; i--)
		printf("%lld", (value >> i) & 1);

	if (size == 0) printf("%d", 0);
	printf("(2)\n");
}


// subnetting�� �ʿ��� ����
int * nw_id;	// ���� ip
long long int IP; // 32bit ip
int subnet_mask;
int subnet_size;
int * host_size_each_subnet;
int * host_requirement; // �� subnet���� �ʿ��� host�� ��(2�� n�����¿��� n�� ��)
int is_ip_subnet = 0;
#define IPV 32 // IPv4�� bit��

// IP 4���� �� ������ ��� ����ó��
void exception_not_qualify_size() {
	printf("* exception : not qualify number of ip sizes \n\n");
	exit(1);
}

// 0~255������ ip�ּҰ��� �ƴ� ��� ����ó��
void exception_exceed_range() {
	printf("* exception : exceed the range (0~255) \n\n");
	exit(1);
}

// �ڷ����� �߸� �Է��� ��� ����ó��
void exception_not_qualify_type() {
	printf("* exception : not qualify type \n\n");
	exit(1);
}

// 127�� �����ϴ� loopback address�� ��� ����ó��
void exception_loopback_address() {
	printf("* exception : this n/w id is a loopback address \n\n");
	exit(1);
}

// ��Ʈ��ũ id�� �߸� �Է��� ���
void exception_nw_id() {
	printf("* exception : intput n/w id is not network id \n");
	printf("but, program keep going by regarding that input n/w id is IP address.\n\n");
}

// ��Ʈ��ũ id�� �߸� �Է����� �Ǵ��ϴ� �Լ�
int is_exception_nw_id(int sm) {
	// sm : subnet mask, in_nw_id : �Է¹��� ��Ʈ��ũ id
	long long int tmp = 1;
	long long int not_SM = (1 << (IPV - sm)) - 1;

	// ��Ʈ��ũ id�� subnet_mast�� ������ �� �߸� �Է��� ��� 1 / �� �Է��� ��� 0
	if ((IP&not_SM) == 0) return 0;
	else return 1;
}

// �� ����� �� ȣ��Ʈ�� ���� ��� �Է����� �ʾҰų� �ʰ��Ͽ� �Է��� ���
void exception_not_qualify_host_size() {
	printf("* exception : not qualify number of host sizes \n\n");
	exit(1);
}

// ���� ���� (�������� ���Ŀ� ����(subnet �� host ��, �迭�� ���)
void bubble_sort(int * arr, int n) {
	int i, j, tmp;
	for (i = n - 1; i>0; i--)
		for (j = 0; j<i; j++)
			if (arr[j]<arr[j + 1]) {
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
			}
}

// �Է� ���� ���ڿ� ip �ּҸ� split�Ͽ� ������ �ٲٴ� �Լ�
int * my_split(char * str, char * token, int subnet_size, int is_nw_id) {
	char * ptr;
	int index_size;

	// ��Ʈ��ũ id�� ���� �۾��� ������, Subnet �� ȣ��Ʈ ���� ���� �۾��� ������ ����
	if (is_nw_id) index_size = 4; // nw_id�ΰ��
	else index_size = subnet_size; // subnet�� ȣ��Ʈ ���� ���

	int * result_split = (int*)malloc(sizeof(int) * index_size);

	ptr = strtok(str, token);
	if (ptr == NULL) exception_not_qualify_size();

	result_split[0] = atoi(ptr); // ascii to Int
	for (int i = 1; i < index_size; i++) {
		ptr = strtok(NULL, token);

		// �� ����ݴ� ȣ��Ʈ�� ���� ��� �Է����� ���� ��� ����ó��
		if (ptr == NULL) exception_not_qualify_host_size();
		result_split[i] = atoi(ptr);
	}

	// ip�ּҿ� ���� �۾��ϴ� ���, ����(0~255)�� �Ѿ�� ����ó��
	if (is_nw_id) {
		for (int i = 0; i < index_size; i++) {
			// 0~255������ ip�ּҰ��� �ƴ� ��� ����ó��
			if (result_split[i] > 255 || result_split[i] < 0) exception_exceed_range();
		}

		if (result_split[0] == 127) exception_loopback_address();
	}
	else { // �� ����ݴ� ȣ��Ʈ�� ���� �ʰ��� �Է��� ��� ����ó��
		ptr = strtok(NULL, token);
		if (ptr != NULL) exception_not_qualify_host_size();
	}
	return result_split;
}

// 4���� ���ݰ��� �޾Ƽ� 32bit������ ��ȯ�ϴ� �Լ�
long long int convert_to_32bit(int * arr) {
	long long int tmp = 1;
	long long int ip = 0;

	int j = 24;
	for (int i = 0; i < 4; i++, j -= 8) {
		long long int arr_tmp = arr[i];
		ip += arr_tmp << j;
	}

	return ip;
}

#define MAX 123456
// �Է� �Լ� & ����� �� ȣ��Ʈ ũ��� �������� ����
// ���� ó���� ���ÿ� ����
//* 1) IP�ּ��� �� ��, 0~255 ������ ���� �ƴ� ���
//* 2) ������ �ƴ� ���ڿ� ���� �ٸ� Ÿ���� �Է��� ���
//* 3) subnet mask ũ�Ⱑ 32�ʰ��� ���
//* 4) 127�� IP�ּҸ� ���� �� : loopback address
//* 5) n/w id�� submask ������ �� �߸� �Է��� ���
void in() {
	char nw_id_[MAX] = "";
	subnet_mask = -1;
	subnet_size = -1;
	char host_size_each_subnet_[MAX] = "";

	printf("********************** input ********************************");
	printf("\n* 1. n/w id : "); gets(nw_id_);
	printf("\n* 2. subnet mask : "); scanf("%d", &subnet_mask);
	printf("\n* 3. number of subnet : "); scanf("%d", &subnet_size); gets();
	printf("\n* 4. number of host at each subnet : "); gets(host_size_each_subnet_);
	printf("********************** finish ********************************\n\n\n");

	// �ڷ����� �߸� �Է��� ��� ����ó��
	if (subnet_mask == -1 || subnet_size == -1) exception_not_qualify_type();

	nw_id = my_split(nw_id_, ".", subnet_size, 1);
	host_size_each_subnet = my_split(host_size_each_subnet_, " ", subnet_size, 0);

	// �������� ����
	bubble_sort(host_size_each_subnet, subnet_size);

	// 32bit ip
	IP = convert_to_32bit(nw_id);

	// ��Ʈ��ũ id�� subnet mask�� ������ �� �߸� �Է��� ��� ����ó��.
	// ��, �׷� ��� ��Ʈ��ũ id�� ��ü ip�� ���� ������.
	if (is_exception_nw_id(subnet_mask)) {
		exception_nw_id();
		is_ip_subnet = 1;
	}

}

// (host ũ�� + 2) ���� ū 2�� ������ �� �߿���, �ּ� ���� �� ������������ ��ȯ�ϴ� �Լ� ȣ��
int search_min(int host_size) {
	int requirement = host_size + 2;
	int i = 0;
	for (i = 0;; i++) {
		if (requirement <= (1 << i))
			break;
	}
	return i;
}

// �� subnet�� �ʿ��� host��(2�� n��) �����ϴ� �Լ�
void store() {
	host_requirement = (int*)malloc(sizeof(int)*subnet_size);
	for (int i = 0; i < subnet_size; i++) {
		host_requirement[i] = search_min(host_size_each_subnet[i]);
	}
}

// subnetting ����(��ȯ���� ù ��° subnet�� ip min ��)
long long int execute() {
	long long int tmp = 1;
	int HB = host_requirement[0]; // host bit
	if (is_ip_subnet) HB++;
	long long int SM = ((tmp << IPV - HB) - 1) << HB;	// subnet mask
	long long int NA = IP & SM;		// network address, IP : 32bit
	return NA + 1;
}

// ù ��° ������� �Ҵ簡���� �ּҰ� ip�� �޾Ƽ�, ������ ������� min,max���� ���ϴ� �Լ�
long long int ** get_min_max(long long int first_subnet_ip_min) {

	// ��ȯ���� ���� ���� �Ҵ�[i][j] : i�� ����� ũ��, j�� �ּҿ� �ִ밪
	long long int ** result = (long long int **)malloc(sizeof(long long int *)*subnet_size);
	for (int i = 0; i<subnet_size; i++)
		result[i] = (int*)malloc(sizeof(int) * 2);

	for (int i = 0; i < subnet_size; i++)
		for (int j = 0; j < 2; j++)
			result[i][j] = -1;

	result[0][0] = first_subnet_ip_min;
	result[0][1] = result[0][0] + (1 << host_requirement[0]) - 3;
	long long int capacity = 1 << (IPV - subnet_mask);
	capacity -= (1 << host_requirement[0]);

	// sum ���� �Ҵ� ������ host�� ������ ũ�� �Ǹ� -1 �Ҵ�
	for (int i = 1; i < subnet_size && capacity > 0; i++) {
		for (int j = 0; j < 2; j++) {
			if (j == 0) result[i][0] = result[i - 1][1] + 3;
			else {
				int h_requirement = (1 << host_requirement[i]);
				result[i][1] = result[i][0] + h_requirement - 3;
				capacity -= h_requirement;
				if (capacity < 0) {
					result[i][1] = -1;
					result[i][0] = -1;
				}
			}
		}
	}
	return result;
}

// 10������ �Է¹޾�, 4���� ip �迭(����)�� ��ȯ�Ͽ� ��ȯ�ϴ� �Լ�
int * convert_to_octet(long long int value) {

	int * ip = (int*)malloc(sizeof(int) * 4);
	int j = 24;
	for (int i = 0; i < 4; i++, j -= 8) {
		ip[i] = (value >> j);
		ip[i] = ip[i] & ((1 << 8) - 1);
	}
	return ip;
}

// ���� ������ 2���� �迭 ��ȯ
int ** convert(long long int ** subnet_ip) {

	// ��ȯ���� ���� ���� �Ҵ�[i][j] : : i=min, max, min, max,,, �ݺ�(�Ҵ�Ұ��� NULL), j=���ݰ�
	int length = 2 * subnet_size;
	int ** result = (int **)malloc(sizeof(int *)*length);
	for (int i = 0; i<length; i++)
		result[i] = (int*)malloc(sizeof(int) * 4);

	int q = 0;
	for (int i = 0; i < subnet_size; i++, q++) {
		if (subnet_ip[i][1] == -1) {
			result[i + q] = NULL;
			result[i + 1 + q] = NULL;
		}
		else {
			result[i + q] = convert_to_octet(subnet_ip[i][0]);
			result[i + 1 + q] = convert_to_octet(subnet_ip[i][1]);
		}
	}
	return result;
}

// ���� ���� ����ϴ� �Լ�
void print_reult(int ** value) {
	// value[i][j] : i=min, max, min, max,,, �ݺ�(�Ҵ�Ұ��� NULL), j=���ݰ�

	printf("*************************************************************\n");

	int q = 0;
	for (int i = 0; i < subnet_size; i++, q++) {
		printf("%d)subnet : ", i + 1);
		if (value[i + q] == NULL) {
			printf("unused");
		}
		else {

			// MIN
			for (int j = 0; j < 4; j++) {
				if (j == 3) printf("%d", value[i + q][j]);
				else printf("%d.", value[i + q][j]);
			}

			// MAX
			printf(" ~ ");
			for (int j = 0; j < 4; j++) {
				if (j == 3) printf("%d(/%d)", value[i + 1 + q][j], (IPV - host_requirement[q]));
				else printf("%d.", value[i + 1 + q][j]);
			}
		}
		printf("\n");
	}
	printf("*************************************************************\n");
}

int main() {

	// Ű���� �Է� �޾�, split�� �̿��� ���� ������ �Է��ϴ� �۾�
	in();

	// �� subnet�� �ʿ��� host��(2�� n��) �����ϴ� �۾�
	store();

	// subnetting ����(��ȯ���� ù ��° subnet�� ip min ��)
	long long int first_subnet_ip_min = execute(subnet_size);

	;	// ������ subnet�� ip�� �ּҰ�, �ִ밪 ��ȯ�ϴ� �Լ� (NULL���� return �Ǹ� �Ҵ� �Ұ��� ũ��)
	long long int ** subnet_ip = get_min_max(first_subnet_ip_min); //[i][j] : i�� ����� ũ��, j�� �ּҿ� �ִ밪
	if (subnet_ip == NULL) printf("�Ҵ� �Ұ��� ũ��");

	// 32��Ʈ�� �������� ��ȯ �Ͽ� ��ȯ �ϴ� �Լ�([i][j] : i=min,mix,min,max... / j=���ݰ�)
	int ** result = convert(subnet_ip);

	// ����ȭ�Ͽ� ����ϴ� �Լ�
	print_reult(result);

	return 0;
}