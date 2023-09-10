int power(int base, int exponent) {
    // ...
}

int main() {
    int base = 2;
    int exponent = 3;
    int result = power(base, exponent);
    return 0;
}






/////////////////////////////////







#include <stdio.h>
 
int main()
{
    int base, exponent;
 
    long long result = 1;
 
    printf("基数: ");
    scanf("%d", &base);
 
    printf("指数: ");
    scanf("%d", &exponent);
 
    while (exponent != 0)
    {
        result *= base;
        --exponent;
    }
    printf("结果：%lld", result);
 
    return 0;
}


////////////////////////////

#include<stdio.h>

int cifang(int n,int k)
{
	
	if(k==0)
		return 1;
	
	else if(k==1)
		return n;

	else
		return n*cifang(n,k-1);
}


int main()
{
	int x,y;//y表示多少次方
	scanf("%d %d",&x,&y);
	printf("%d\n",cifang(x,y));
	return 0;
}