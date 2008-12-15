//g++ testBALprincipes.cpp -ldeepsee -ldw -lelf -ldl && ./a.out

#define DEEPSEE_LOGGER_ENABLE 1
#define DEEPSEE_LOGGER_USE 1
#include <deepsee/deepsee.h>

#include <iostream>


DS_INIT_DEEPSEE_FRAMEWORK();

using namespace std;
namespace BAL{

	class BIBase {
		public:
			BIBase(){}
	};

	class BDBase {
		public:
			BDBase(){}
	};

	class BIWKAL {
		public:
			BIWKAL(){}
	};

	class BITata : public BIBase {
		public:
			BITata(){DS_TRC("tata");}
	};
	class BITiti : public BIBase {
		public:
			BITiti(){DS_TRC("BItiti");}
			virtual void prout()=0;
			virtual void prot()=0;
			virtual BITiti*	getTitiPtr()=0;
			virtual BITiti&	getTitiRef()=0;
	};
	class BDTiti : public BDBase {
		public:
			BDTiti(){DS_TRC("DS: titi"<<this);}
			BDTiti(const BDTiti& bd):BDBase(bd){DS_TRC("DS: titi copy"<<this);}
			~BDTiti(){DS_TRC("DS: ~Titi"<<this);}
	};
	class BDToto : public BDBase {
		public:
			BDToto(){DS_TRC("DS: toto"<<this);}
			BDToto(const BDToto& bd):BDBase(bd){DS_TRC("DS: toto copy"<<this);}
			~BDToto(){DS_TRC("DS: ~Toto"<<this);}
	};
	
	template<class T> class BIToto : public BAL::BIBase {
		public:
			BIToto(){DS_TRC("BIToto");}
			virtual T*	getT()=0;
			virtual void printT()=0;
			virtual void setT(T* t_)=0;
	};





    class BINoncopyable {
		private:
	        BINoncopyable(const BINoncopyable&);
	        BINoncopyable& operator=(const BINoncopyable&);
	    protected:
	        BINoncopyable() {cout<<"BINoncopyable"<<endl; }
	        ~BINoncopyable() { }
	   	};
};


#if 1
namespace BAL{
#define BCTata Tata
	class Tata : public BAL::BITata {
		public:
			Tata(){DS_TRC("WK:: tata");}
	};
}

namespace WebCore {
	using BAL::BCTata;
}
#endif

#if 1
//implementation WK
//

	namespace WTFNoncopyable {
	    class Noncopyable :public BAL::BINoncopyable{
		private:
	        Noncopyable(const Noncopyable&);
	        Noncopyable& operator=(const Noncopyable&);
	    protected:
	        Noncopyable() {cout<<"WKNoncopyable"<<endl; }
	        ~Noncopyable() { }
	   	};

	} // namespace WTFNoncopyable

	using WTFNoncopyable::Noncopyable;

namespace BAL{
#define BCTata Tata
	class Tata : public BAL::BITata {
		public:
			Tata(){DS_TRC("WK:: tata");}
	};

#define BCTiti Titi
	class Titi : public Tata, public BAL::BITiti,public BAL::BDTiti /*,Noncopyable*/{
		public:
			Titi(){cout<<"WKtiti"<<endl;}
//			Titi*	getTitiPtr(){return this;}
			Titi*	getTitiPtr(){cout<<"WK ptr"<<endl;return this;}
			Titi&	getTitiRef(){cout<<"WK ref"<<endl;return *this;}
			void prout(){cout<<"WK prout"<<endl;}
			void prot(){cout<<"WK prot"<<endl;}

//			static Titi&	getSingletonRef(){return singleton;}
//			static Titi*	getSingletonPtr(){return &singleton;}

//		static Titi singleton;
	};

#define BCToto Toto
	template<class T> class Toto : public BAL::BIToto<T>,public BAL::BDToto {
		public:
			Toto(){DS_TRC("coucou");}
			T*	getT(){return t;}
			void printT(){DS_TRC("t="<<t);}
			void setT(T* t_){t=t_;}
		protected:
			T* t;
	};

};

namespace WebCore {
	using BAL::BCTata;
	using BAL::BCTiti;
	using BAL::BCToto;
}

#else
namespace BAL{
#if 0
#define BCTata BCTataSDL
#define BCTataSDL Tata
	class BCTataSDL : public BITata {
		public:
			Tata(){DS_TRC("SDL :tata");}
			
	};
#endif
#define BCTiti BCTitiSDL
#define BCTitiSDL Titi
	class BCTitiSDL : public BCTata, public BITiti,public BDTiti{
		public:
			BCTitiSDL(){cout<<"BCTitiSDL"<<this<<endl;}
			virtual void prout(){cout<<"BCSDL prout"<<endl;};
			virtual void prot(){cout<<"BCSDL prot"<<endl;};
//			virtual BITiti*	getTitiPtr(){return this;}
			virtual BCTitiSDL*	getTitiPtr(){cout<<"BCSDL ptr"<<endl;return this;}
//			virtual BITiti&	getTitiRef(){return *this;}
			virtual BCTitiSDL&	getTitiRef(){cout<<"BCSDL ref"<<endl;return *this;}
	};
#define BCToto BCTotoSDL
#define BCTotoSDL Toto
	template<class T> class BCTotoSDL : public BAL::BIToto<T>,public BAL::BDToto {
		public:
			BCTotoSDL(){DS_TRC("SDL");}
			T*	getT(){return t;}
			void printT(){DS_TRC("t="<<t);}
			void setT(T* t_){t=t_;}
		protected:
			T* t;
	};

}
namespace WebCore{
	using BAL::Titi;
	using BAL::Toto;
}
#endif

using namespace WebCore;
int main(int argc,char** argv)
{
cout<<"ascii"<<endl;
wcout<<L"toto"<<endl;
	DS_TRC("début");
//	Titi&	titiSingletonRef=Titi::getSingletonRef();
//	Titi*	titiSingletonPtr=Titi::getSingletonPtr();
	Titi*	titiPtr=new Titi();
	titiPtr->prout();
	titiPtr->prot();
	Titi	titiCopy=titiPtr->getTitiRef();
	Titi*	titiPtr2=titiPtr->getTitiPtr();

	BAL::BCTiti * t=new BAL::BCTiti();
	t->prout();
	delete(t);
	delete(titiPtr);

DS_TRC("Template");
	Titi*	titi2=new Titi();
	BCToto<Titi>	toto;
	toto.setT(titi2);
	toto.printT();
	DS_TRC("getT="<<toto.getT());
	delete(titi2);

DS_CLEAN_DEEPSEE_FRAMEWORK();
}



