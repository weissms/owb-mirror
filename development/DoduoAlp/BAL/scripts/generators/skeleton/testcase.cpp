//generate its xml by: 
//	gccxml -fxml-start="Foobar" -fxml=testcase.xml testcase.cpp
//	gccxml -fxml=testcaseFull.xml testcase.cpp

class D1;
class D2{
	public:
		D2(D1*){}
};
namespace Foobar{
	class C1;
	class C2;
	class C3{
		public:
			C3(C2*){}
			void testArgInt(int i);
			void testArgIntRef(int& refI);
			void testArgIntRefConst(const int& refI);
			void testArgIntPtr(int* ptrI);
			int testRetIntArgChar(char c);
			void testArgCharArray(char c[10]);
			void testArgs(int i1,int i2,char c1,char c2);
			void testArgsInit(int i1,int i2=0,int* ptrI=0);
			static const int staticIntField=0;
			class C31:public D2{
				public:
					C31():D2(0){};
					explicit C31(int):D2(0){}
					~C31(){}
					virtual void virtualFunc(){};
					virtual void pureVirtualFunc()=0;
					virtual void pureVirtualConstFunc()const =0 ;
					static int staticReturnInt(){};
					int* returnIntPtr(){};
					int& returnIntRef(){};
					const int& returnIntRefConst(){};
					void constFunc()const{}
				protected:
					void embeddedIntoC3(){};
			};
	};
	class C2{
		public:
			enum C2Enum{c2Enum0=0,c2Enum1,c2Enum2,c2Enum3=33};
			C2(C2Enum toto=c2Enum1){}
	};
	int functionTest(int& refI,const char* string,int i=15);
	class C1{
		C1(C2::C2Enum val){}
	};
};
