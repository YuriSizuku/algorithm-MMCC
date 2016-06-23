int main(int arg1,float arg2)
{//main fuctions
	int a;
	int b;
	int c;
	float f1;
	float f2[3][3];
	a=1;
	b=a;
	c=(a+a*b)+a/b*a;
	a=main(b,f2[2][1]);
	if(a>=b)
	{
		if(c>=a)
			c=a;
		else
		{
			while(a<=b)
			{
				a=a+1;
				break;
			}
		}
		return 3;
	}
	f2[2][2]=3.2;
	f1=f2[2][2]+3.4;
	return 0;
}