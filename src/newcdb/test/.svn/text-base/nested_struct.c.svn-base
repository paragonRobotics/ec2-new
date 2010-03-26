
int main()
{
	volatile struct
	{
		struct s
		{
			int		ii;
			float	fp[4];
		} XYZ;
		int i;
		int j;
	} ABC;

	while(1)
	{
		ABC.i = 4;
		ABC.XYZ.fp[2] = 3.14;
	}
}
