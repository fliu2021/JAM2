#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include <iomanip>
#include <sstream>
#include <ctime>

#ifdef USE_ROOT
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#endif

#include <jam2/JAM.h>
//#include <jam2/initcond/InitialCondition.h>
//#include <jam2/hadrons/JamStdlib.h>

using namespace std;
using namespace Pythia8;
using namespace jam2;

//void addhist(Analysis* ana);

//#include "book/Book1.h"
#include <Pythia8/Basics.h>


#ifdef USE_ROOT
class RootTrack{
  RootTrack(int id, double* x, double* p) {
    pid=id;
    rx=x[1]; ry=x[2]; rz=x[3];
    px=p[1]; py=p[2]; pz=p[3];
  }
  int pid;
  int coll; // =1 or -1:participant (no collision)
  float rx, ry, rz, rt;
  float px, py, pz, E;
};

class TTreeEvent{
public:
  int Npart, mul;
  float b;
  std::vector<int> pid;
  std::vector<int> coll; // =1 or -1:participant (no collision)
  //std::vector<float> mass;
  std::vector<float> rx, ry, rz, rt;
  std::vector<float> px, py, pz, E;

  void init(int npart, int n,double b0) {
    Npart=npart; mul=n; b = b0;
  }
  void fill(int id, int c, double* x, double* p) {
    pid.push_back(id);
    coll.push_back(c);
    rx.push_back(x[1]);
    ry.push_back(x[2]);
    rz.push_back(x[3]);
    rt.push_back(x[0]);
    px.push_back(p[1]);
    py.push_back(p[2]);
    pz.push_back(p[3]);
    E.push_back(p[0]);
    //tracks.push_back(RootTrack(id,x,p));
  }
  void clear() {
    pid.clear(); coll.clear();
    rx.clear(); ry.clear(); rz.clear(); rt.clear();
    px.clear(); py.clear(); pz.clear(); E.clear();
  }

  //define Branch
  void branch(TTree& tree) {

    // total number of particles in the current event
    tree.Branch("mul", &mul, "mul/I");

    // impact parameter
    tree.Branch("b", &b, "b/F"); 

    // number of participants estimated by the Glauber theory (not the real participant number in the JAM simulation)
    tree.Branch("Npart", &Npart, "Npart/I");

    // particle PDG ID 
    tree.Branch("pid", &pid);

    // If coll= 1 or -1, the particle did not collide, i.e. spectator particle.
    // Real number of participants can be estimated by subtracting the number of
    // particle with coll=1,or-1 from total multiplicity 'mul'
    tree.Branch("coll", &coll);

    // particle coordinate.
    tree.Branch("x", &rx);
    tree.Branch("y", &ry);
    tree.Branch("z", &rz);
    tree.Branch("t", &rt);

    // particle energy-momentum.
    tree.Branch("px",&px);
    tree.Branch("py",&py);
    tree.Branch("pz",&pz);
    tree.Branch("E", &E);
  }

  //stdf::vector<RootTrack> tracks;
};

TTreeEvent treeEvent;

  
void outputPhaseSpaceRoot(JAM* jam,int opt)
{
  list<EventParticle*>&  plist = jam->getEvent();
  int nv = plist.size();
  int npartG = jam->getGlauberParticipants();
  double b = jam->impactParameter();
  treeEvent.clear();
  treeEvent.init(npartG, nv, b);
  double rr[4], pp[4];
  for(auto& p: plist) {
    // opt=1: getV(): last collision point  opt=0: getR():last particle coordinate
    Vec4 r = opt == 0 ? p->getR() : p->getV();
    for(int j=0;j<4;j++) {
      rr[j]=r[j];
      pp[j]=p->getP(j);
    }
    if(p->getStatus()>0)
    treeEvent.fill(p->getID(),p->getNColl(), rr, pp);
  }

}
#endif


string date()
{
      /*
      time_t t=time(nullptr);
      const tm* lt = localtime(&t);
      std::stringstream s;
      s<<"20";
      s<<lt->tm_year-100; //100を引くことで20xxのxxの部分になる
      s<<"-";
      s<<lt->tm_mon+1; //月を0からカウントしているため
      s<<"-";
      s<<lt->tm_mday; //そのまま
      //result = "2015-5-19"
      std::string result = s.str();
      */

    time_t timer;        /* 時刻を取り出すための型（実際はunsigned long型） */
    struct tm *local;    /* tm構造体（時刻を扱う */

    /* 年月日と時分秒保存用 */
    int year, month, day, hour, minute, second;
    timer = time(NULL);        /* 現在時刻を取得 */
    local = localtime(&timer);    /* 地方時に変換 */
    /* 年月日と時分秒をtm構造体の各パラメタから変数に代入 */
    year = local->tm_year + 1900;        /* 1900年からの年数が取得されるため */
    month = local->tm_mon + 1;        /* 0を1月としているため */
    day = local->tm_mday;
    hour = local->tm_hour;
    minute = local->tm_min;
    second = local->tm_sec;

    std::stringstream s;
    //s<< year << month << day << hour << minute << second;
    //s<< year <<"Y" << month << "M" << day << "D" << hour <<"H" ;

    s << year <<" " << setw(2) << setfill('0') << month
      << ":" << day << ":" << hour
      << ":"<< minute
      << ":"<< second;

    //cout << resetiosflags(ios_base::floatfield);

   cout << s.str()<<endl;

    //printf("%dy%dM%dD %dH%dM%dS\n", year, month, day, hour, minute, second);
    printf("%d/%d/%d %02d:%02d:%02d\n",year,month,day,hour,minute,second);

    return s.str();
}

class OutPutPhaseSpaceJAM
{
private:
  ofstream ofs; // for phase space data output
  string outputFname;

public:
  OutPutPhaseSpaceJAM(JAM* jam, string fname, int nev, int opt=0) {
    outputFname = fname;

    // filename with data.
    if(opt) outputFname += date();

    ofs.open(outputFname.c_str());

    ofs << "# " << nev << setw(10) << " " << jam->initcond()->ycm()
        << setw(8) << jam->initcond()->gammaA()
        << setw(8) << jam->initcond()->gammaB()
        << setw(8) << 3
        <<endl;
  }

void close() {
    ofs.close();
    //system(("gzip -f "+outputFname).c_str());
}

void fill(int iev, int mass, JAM* jam,int opt)
{
  list<EventParticle*>&  plist = jam->getEvent();
  int nv = plist.size();
  int ncoll   = jam->getNColl();    // number of total collisions
  int ncollBB = jam->getNCollBB();  // number of total baryon-baryon collisions
  int ncollG  = jam->getGlauberColl(); // number of predicted collisions by Glauber  model
  int npartG  = jam->getGlauberParticipants(); // number of participants predicted by Glauber model
  int nonpart=0;
  int nmeson=0;
  for(auto jp=plist.begin(); jp != plist.end(); jp++) {
    if(abs((*jp)->getNColl())==1 && (*jp)->getParent()==0) nonpart++;
    if((*jp)->baryon() == 0) nmeson++;
  }
  //int nbaryon = nv - nmeson;
  int npart = mass - nonpart;   // number of actual nucleon participants
  double b = jam->impactParameter();

  ofs << "# " << iev << setw(12) << nv << setw(8) << ncollG
      << setw(15) << npartG
      << setw(18) << b
      << setw(15) << npart
      << setw(8) << ncoll
      << setw(8) << ncollBB
      << endl;
  Vec4 ptot=0.0;
  for(auto& p: plist) {
    // getV: last collision point  getR:real particle coordinate
    Vec4 r = opt == 0 ? p->getR() : p->getV();
    ofs << setw(3) << p->getStatus()
        << setw(12) << p->getID()
        //<< setw(15) << scientific << p->getNColl()
        << setw(15) << p->getNColl()
        << setw(15) << fixed << p->getMass()
        << scientific
	<< setw(16) << setprecision(8) << p->getPx()
        << setw(16) << setprecision(8) << p->getPy()
        << setw(16) << setprecision(8) << p->getPz()
        << setw(16) << setprecision(8) << p->getPe()
        << setw(16) << setprecision(8) << r[1]
        << setw(16) << setprecision(8) << r[2]
        << setw(16) << setprecision(8) << r[3]
        << setw(16) << setprecision(8) << r[0]
        //<< setw(16) << setprecision(8) << p->TimeLastColl()
        << setw(16) << setprecision(8) << p->getTf()
	<< endl;
    ptot += p->getP();
    /*
    if(p->baryon()!=0) {
    cout << setw(3) << p->getID()
        << setw(15) << fixed << p->getMass()
        << setw(15) << fixed << p->getEffectiveMass()
        << setw(15) << fixed << p->pots()
        << setw(15) << fixed << p->facPotential(0)
	<<endl;
    }
    */
  }
}

};

//The class 'UserInitialCondition' is in the initcond/InitialCondition.h
class MyInitialCondition: public UserInitialCondition
{
private:
  int option;
  ifstream fn;
  string fname="../2020PbPb.rdh.000021.phasespace0.v2.dat";
public:
  //MyInitialCondition(Pythia8::Settings* s, JamParticleData* pd,Pythia8::Rndm* r)
  MyInitialCondition(int opt, Pythia8::Info* i, JamParticleData* pd)
    :UserInitialCondition(i,pd) { 
    option=opt;
  }
  ~MyInitialCondition() {;}
  void init();
  void generate(Collision* event,int mode=0);
  void readFile(Collision* event);
};

void MyInitialCondition::init()
{
  //some initializations.
  // If energy of two-body collision is large, we need to initialize pythia8.
  // set maximum possible collision energy.
  double eCMmax=20.0;
  settings->parm("Beams:eCM",eCMmax);

}

void MyInitialCondition::readFile(Collision* event)
{
  // open file
  fn.open(fname.c_str(),ios_base::in);
  if(!fn) {
    cerr << "Error unable to open file " << fname << endl;
    exit(1);
  }
  string templine;
  int np=0,nev=1;
  getline(fn,templine);
  istringstream is0(templine);
  is0 >> np >>  nev;

  int lineposition=0;
  // loop over all particles.
  while(getline(fn,templine)) {
    lineposition++;
    int icomment1=templine.find('#');
    if(icomment1>=0) continue;
    if(lineposition==np) break; // select only the first event

    istringstream is(templine);
    int ks,kf;
    double m,px,py,pz,x,y,z,t;
    is >> ks >> kf >> px >> py >> pz >> m >> x >> y >> z >> t;
    //cout << "line= "<< lineposition << " ks= "<< ks <<endl;
    //cout << templine <<endl;
    if(ks == -999) break; // end

    int id=kf;
    int sign= kf > 0 ? 1 : -1;
    // N(1520) 1214,2124 -> 102114,102214
    switch(abs(kf)) {
      case   10220: id=9000221; break;  // f_0(500) (sigma-meson)
      case   20221: id=100221;  break;  // eta(1295)
      case   20211: id=100211;  break;  //  pi(1300)+
      case   20111: id=100111;  break;  //  pi(1300)0
      case - 20211: id=-100211; break;  //  pi(1300)-
      case   30221: id=10221;   break;  // f_0(1300) -> f_0(1370)
      case   40213: id=30213;   break;  // rho(1700)+ 
      case   40113: id=30113;   break;  // rho(1700)0
      case  -40213: id=-30213;  break;  // rho(1700)- 
      case   50223: id=9000223; break;  //  f_1(1510) 
      case   40313: id=30313;   break;  //  K*(1680)0
      case   40323: id=30323;   break;  //  K*(1680)+
      case  -40323: id=-30323;  break;  //  K*(1680)-
      case   60223: id=30223;   break;  // omega(1600) -> omega(1650)
      case   30333: id=100333;  break;  // phi(1680) 

      case 12112: id=202112*sign; break; // N(1440)0   12112  1.4400   350.00  202112 1/2+
      case 1214:  id=102114*sign; break; // N(1520)0    1214  1.5200   120.00  102114 3/2-
      case 22112: id=102112*sign; break; // N(1535)0   22112  1.5350   150.00  102112 1/2-
      case 32112: id=122112*sign; break; // N(1650)0   32112  1.6500   150.00  122112 1/2-
      case 2116 : id=102116*sign; break; // N(1675)0    2116  1.6750   150.00  102116 5/2-
      case 12116: id=202116*sign; break; // N(1680)0   12116  1.6800   130.00  202116 5/2+
      case 21214: id=112114*sign; break; // N(1700)0   21214  1.7000   100.00  112114 3/2-
      case 42112: id=212112*sign; break; // N(1710)0   42112  1.7100   100.00  212112 1/2+
      case 31214: id=212114*sign; break; // N(1720)0   31214  1.7200   150.00  212114 3/2+
      case 41214: id=102118*sign; break; // N(1990)0   41214  1.9900   350.00  102118 7/2+
      case 11218: id=102118*sign; break; // N(1990)0   41214  1.9900   350.00  102118 7/2+
      case 1218: id= 11211*sign;  break;  // N(2190)0    1218  2.1900   450.00  112118 7/2-
      // new N(1895)0          1.8950   450.00  302112 1/2-
      // new N(1900)0          1.9000   450.00  302114 7/2-

      case 12212: id=202212*sign;break; // N(1440)+   12212  1.4400   350.00 202212
      case 2124 : id=102214*sign;break; // N(1520)+    2124  1.5200   120.00 102214
      case 22212: id=102212*sign;break; // N(1535)+   22212  1.5350   150.00 102212
      case 32212: id=122212*sign;break; // N(1650)+   32212  1.6500   150.00 122212
      case 2216 : id=102216*sign;break; // N(1675)+    2216  1.6750   150.00 102216
      case 12216: id=202216*sign;break; // N(1680)+   12216  1.6800   130.00 202216
      case 22124: id=112214*sign;break; // N(1700)+   22124  1.7000   100.00 112214
      case 42212: id=212212*sign;break; // N(1710)+   42212  1.7100   100.00 212212
      case 32124: id=212214*sign;break; // N(1720)+   32124  1.7200   150.00 212214
      case 42124: id=102218*sign;break; // N(1990)+   42124  1.9900   350.00 102218
      case 12128: id=102218*sign;break; // N(1990)+   42124  1.9900   350.00 102218
      case 2128 : id=112218*sign;break; // N(2190)+    2128  2.1900   450.00 112218
      // new N(1895)+          1.8950   450.00 302212   1/2-
      // new N(1900)+          1.9000   450.00 302214   7/2-

      // Delta*
      case 31114 : id=201114*sign;break ; // Delta(1600)-
      case  1112 : id=111112*sign;break ; // Delta(1620)-
      case 11114 : id=121114*sign;break ; // Delta(1700)-
      case 11112 : id=231112*sign;break ; // Delta(1900)-
      case  1116 : id=211116*sign;break ; // Delta(1905)-
      case 21112 : id=221112*sign;break ; // Delta(1910)-
      case 21114 : id=221114*sign;break ; // Delta(1920)-
      case 11116 : id=221116*sign;break ; // Delta(1930)-
      case  1118 : id=201118*sign;break ; // Delta(1950)-

      case 32114 : id=202114*sign;break ; // Delta(1600)0
      case  1212 : id=112112*sign;break ; // Delta(1620)0
      case 12114 : id=122114*sign;break ; // Delta(1700)0
      case 11212 : id=232112*sign;break ; // Delta(1900)0
      case  1216 : id=212116*sign;break ; // Delta(1905)0
      case 21212 : id=222112*sign;break ; // Delta(1910)0
      case 22114 : id=222114*sign;break ; // Delta(1920)0
      case 11216 : id=222116*sign;break ; // Delta(1930)0
      case  2118 : id=202118*sign;break ; // Delta(1950)0

      case 32214 : id=202214*sign;break ; // Delta(1600)+
      case  2122 : id=112212*sign;break ; // Delta(1620)+
      case 12214 : id=122214*sign;break ; // Delta(1700)+
      case 12122 : id=232212*sign;break ; // Delta(1900)+
      case  2126 : id=212216*sign;break ; // Delta(1905)+
      case 22122 : id=222212*sign;break ; // Delta(1910)+
      case 22214 : id=222214*sign;break ; // Delta(1920)+
      case 12126 : id=222216*sign;break ; // Delta(1930)+
      case  2218 : id=202218*sign;break ; // Delta(1950)+

      case 32224 : id=202224*sign;break ; // Delta(1600)++
      case  2222 : id=112222*sign;break ; // Delta(1620)++
      case 12224 : id=122224*sign;break ; // Delta(1700)++
      case 12222 : id=232222*sign;break ; // Delta(1900)++
      case  2226 : id=212226*sign;break ; // Delta(1905)++
      case 22222 : id=222222*sign;break ; // Delta(1910)++
      case 22224 : id=222224*sign;break ; // Delta(1920)++
      case 12226 : id=222226*sign;break ; // Delta(1930)++
      case  2228 : id=202228*sign;break ; // Delta(1950)++

      // Lambda*
      case  13122 : id=102132*sign;break ; // Lambda(1405)
      case   3124 : id=102134*sign;break ; // Lambda(1520)
      case  23122 : id=203122*sign;break ; // Lambda(1600)
      case  33122 : id=103122*sign;break ; // Lambda(1670)
      case  13124 : id=103124*sign;break ; // Lambda(1690)
      case  43122 : id=123122*sign;break ; // Lambda(1800)
      case  53122 : id=213122*sign;break ; // Lambda(1810)
      case   3126 : id=203126*sign;break ; // Lambda(1820)
      case  13126 : id=103126*sign;break ; // Lambda(1830)
      case  23124 : id=213124*sign;break ; // Lambda(1890)
      case   3128 : id=302138*sign;break ; // Lambda(2100)
      case  23126 : id=213126*sign;break ; // Lambda(2110)

      // Sigma(1385)-    3114  1.3872    39.40  3114
      // Sigma(1385)0    3214  1.3837    36.00  3214
      // Sigma(1385)+    3224  1.3828    35.80  3224

     case 13112: id=203112*sign;break ; // Sigma(1660)-   13112  1.6600   100.00  203112
     case 13212: id=203212*sign;break ; // Sigma(1660)0   13212  1.6600   100.00  203212
     case 13222: id=203222*sign;break ; // Sigma(1660)+

     case 13114: id=103114*sign;break ; // Sigma(1670)-   13114  1.6700    60.00 103114
     case 13214: id=103214*sign;break ; // Sigma(1670)0   13214  1.6700    60.00 103214
     case 13224: id=103224*sign;break ; // Sigma(1670)+   13224  1.6700    60.00 103224

     case  23112: id=113112*sign;break ; // Sigma(1750)-   23112  1.7500    90.00 113112
     case  23212: id=113212*sign;break ; // Sigma(1750)0   23212  1.7500    90.00 113212
     case  23222: id=113222*sign;break ; // Sigma(1750)+   23222  1.7500    90.00 113222

     case 3116:   id=103116*sign;break ; // Sigma(1775)-    3116  1.7750   120.00 103116
     case 3216:   id=103216*sign;break ; // Sigma(1775)0    3216  1.7750   120.00 103216
     case 3226:   id=103226*sign;break ; // Sigma(1775)+    3226  1.7750   120.00 103226

     // new Sigma(1910)-    203114
     // new Sigma(1910)0    203124
     // new Sigma(1910)+    203224

    case 13116:  id=203116*sign;break ; // Sigma(1915)- 1.915 120.00
    case 13216:  id=203216*sign;break ; // Sigma(1915)0
    case 13226:  id=203226*sign;break ; // Sigma(1915)+

    case 23114:  id=113114*sign;break ; // Sigma(1940)- 1.94 220.00
    case 23214:  id=113214*sign;break ; // Sigma(1940)0
    case 23224:  id=113224*sign;break ; // Sigma(1940)+

    // new Sigma(2100)-     103118
    // new Sigma(2100)0     103218
    // new Sigma(2100)+     103228

    // new Sigma(2110)-    103112
    // new Sigma(2110)0    103122
    // new Sigma(2110)+    103222

    case  3118:  id=203118*sign;break ; // Sigma(2030)- 2.03 180.00
    case  3218:  id=203218*sign;break ; // Sigma(2030)0
    case  3228:  id=203228*sign;break ; // Sigma(2030)+

    // 3314    3314    // Xi(1530)-
    // 3324    3324    // Xi(1530)0
    case 13314: id= 103314*sign; break ; // Xi(1830)-
    case  3316: id= 203316*sign; break ; // Xi(2030)-
    case 13312: id= 113316*sign; break ; // Xi(1690)-
    case 23312: id= 213312*sign; break ; // Xi(1950)-

    case 13324: id= 103324*sign; break; // Xi(1830)0
    case 3326 : id= 203326*sign; break; // Xi(2030)0
    case 13322: id= 113326*sign; break; // Xi(1690)0
    case 23322: id= 213322*sign; break; // Xi(1950)0

    }

    if(abs(id) < 10) {
      cout << " kf= "<< kf << " id= " << id <<endl;
      exit(1);
    }

    // find this particle in the JAM particle list.
    ParticleDataEntryPtr pa= jamParticleData->find(id);

    // energy.
    double e = sqrt(m*m + px*px + py*py + pz*pz);
    Vec4 p(px,py,pz,e);
    Vec4 r(x,y,z,t);

    EventParticle* cp = new EventParticle(id,m,r,p,pa);
    cp->setPID(jamParticleData->pid(abs(id)));

    // compute decay time if it is resonance.
    double dect = jamParticleData->lifeTime(pa,m,e);
    cp->setLifeTime(t+dect);
    
    // put this particle into the particle list.
    event->setPList(cp);
  }

  fn.close();

}

// put your particle list for the initial condition for JAM.
void MyInitialCondition::generate(Collision* event,int )
{
  if(option==11) {
    readFile(event);
    return;
  }

  int n=100;
  // loop over all particles.
  for(int i=0; i<n;i++) {

    // Particle PDG ID.
    int id=2212;

    // find this particle in the JAM particle list.
    ParticleDataEntryPtr pa= jamParticleData->find(id);

    // set particle coordinate.
    double x = -10.0 + 20*rndm->flat();
    double y = -10.0 + 20*rndm->flat();
    double z = -10.0 + 20*rndm->flat();
    double t = 0.0;
    Vec4 r(x,y,z,t);

    // set particle momentum.
    double px = -10.0 + 20*rndm->flat();
    double py = -10.0 + 20*rndm->flat();
    double pz = -10.0 + 20*rndm->flat();

    // particle mass.
    double m=0.938;

    // energy.
    double e = sqrt(m*m + px*px + py*py + pz*pz);
    Vec4 p(px,py,pz,e);

    EventParticle* cp = new EventParticle(id,m,r,p,pa);
    cp->setPID(jamParticleData->pid(abs(id)));

    // compute decay time if it is resonance.
    double dect = jamParticleData->lifeTime(pa,m,e);
    cp->setLifeTime(t+dect);
    
    // put this particle into the particle list.
    event->setPList(cp);
  }

}


class ParticleMult
{
private:
  int nevent;
  int *npar;
  int xpar;
  const int nhist=24;
public:
  ParticleMult() {
    nevent=0;
    xpar=0;
    npar = new int [nhist];
    for(int i=0;i<nhist;i++) npar[i]=0;
  }
  void fill(JAM* jam);
  void print(int n);
};

void ParticleMult::fill(JAM* jam)
{
  list<EventParticle*>::const_iterator jp;
  list<EventParticle*>&  plist = jam->getEvent();
  nevent++;
  xpar += plist.size();
  for(jp=plist.begin(); jp != plist.end(); jp++) {
    int   id  = (*jp)->getID();
    int ih=-1;
    switch (id) {
     case  2212 : ih=0; break;
     case -2212 : ih=1; break;
     case -211  : ih=2; break;
     case  211  : ih=3; break;
     case  111  : ih=4; break;
     case -321  : ih=5; break;
     case  321  : ih=6; break;
     case  311  : 
     case -311  : ih=7; break; // K0
     case  310  : ih=16; break; // K_S0
     case  130  : ih=17; break; // K_L0
     case  3122 : ih=8; break;  // Lambda
     case -3122 : ih=9; break;  // Lambdabar
     case  3212 : ih=14; break; // Sigma0
     case -3212 : ih=15; break; // Sigma0bar
     case  3112 : ih=10; break; // Sigma-
     case -3112 : ih=11; break; // Sigma-bar
     case  3222 : ih=12; break; // Sigma+
     case -3222 : ih=13; break; // Sigma+bar
     case  3312 : ih=18; break; // Xi-
     case -3312 : ih=19; break; // Xi+bar
     case  3322 : ih=20; break; // Xi0
     case -3322 : ih=21; break; // Xi0bar
     case  3334 : ih=22; break; // Omega-
     case -3334 : ih=23; break; // Omega+bar

     default: ih=-1;
    }
    if(ih>=0) npar[ih]++;
  }
}
void ParticleMult::print(int oversample)
{
  double wei = 1.0/nevent/oversample;
  cout << "total particle = " << xpar*wei << endl;;
  cout << "proton = " << npar[0]*wei << endl;;
  cout << "anti-proton = " << npar[1]*wei << endl;;
  cout << "pi- = " << npar[2]*wei << endl;
  cout << "pi+ = " << npar[3]*wei << endl;
  cout << "pi0 = " << npar[4]*wei << endl;
  cout << "K-  = " << npar[5]*wei << endl;
  cout << "K+  = " << npar[6]*wei << endl;
  cout << "K0+K0bar= " << npar[7]*wei << endl;
  cout << "K_S0= " << npar[16]*wei << endl;
  cout << "K_L0= " << npar[17]*wei << endl;
  cout << "Lambda= " << npar[8]*wei << endl;
  cout << "Lambdabar= " << npar[9]*wei << endl;
  cout << "Sigma-= " << npar[10]*wei << endl;
  cout << "anti-Sigma+= " << npar[11]*wei << endl;
  cout << "Sigma+= " << npar[12]*wei << endl;
  cout << "ani-Sigma+= " << npar[13]*wei << endl;
  cout << "Sigma0= " << npar[14]*wei << endl;
  cout << "ani-Sigma0= " << npar[15]*wei << endl;
  cout << "Xi-= " << npar[18]*wei << endl;
  cout << "ani-Xi+= " << npar[19]*wei << endl;
  cout << "Xi0= " << npar[20]*wei << endl;
  cout << "ani-Xi0= " << npar[21]*wei << endl;
  cout << "Omega= " << npar[22]*wei << endl;
  cout << "ani-Omega= " << npar[23]*wei << endl;
}

class MyHist
{
private:
    double ymin,ymax,xmin,xmax;
    int    ny,nx;
    int    np;
    static const int    nhist1=13;
    double pmin,pmax;
    double dy, dx,dp;
    double eCM, pCM;
    int nhsit1;
    Hist  *hist1;
    Hist  *hist2, *hist3, *hist4;
    Hist  *histc, *histpt;
public:
    MyHist(double y1=-5.0, double y2=5.0);
    ~MyHist();
    void init(double ecm);
    void fill(JAM* event);
    void print(int nev);
};

MyHist::MyHist(double y1, double y2)
{
    ymin =  y1;
    ymax =  y2;
    ny = 70;
    dy = (ymax - ymin)/ny;

    xmin=0.0;
    xmax=1.0;
    nx = 50;
    dx = (xmax - xmin)/nx;

    np = 30;
    pmin = 0.0;
    pmax = 3.0;
    dp = (pmax - pmin ) /np;
}

MyHist::~MyHist()
{
    delete [] hist1;
    delete [] hist2;
    delete [] hist3;
    delete [] hist4;
    delete [] histc;
    delete [] histpt;
}

void MyHist::init(double ecm)
{
    string pa[nhist1]={"proton", "pbar", "pi-", "pi+","pi0","K-","K+","Lambda","netp","Sigma-","Sigma0","Sigma+","K0+K0bar"};

    eCM=ecm;
    double mA=0.938, mB=0.938;
    pCM  = 0.5 * sqrt( (eCM + mA + mB)*(eCM - mA - mB)*(eCM - mA + mB)*(eCM + mA - mB) ) / eCM;
    histc = new Hist[2];
    histc[0].book("dndeta charged",ny,ymin,ymax);
    histc[1].book("dndeta NSD charged",ny,ymin,ymax);

    hist1 = new Hist[nhist1];
    hist2 = new Hist[nhist1];
    hist3 = new Hist[nhist1];
    hist4 = new Hist[nhist1];
    histpt = new Hist[nhist1];
    for(int i=0;i<nhist1;i++) {
      hist1[i].book("dndy"+pa[i],ny,ymin,ymax);
      hist2[i].book("dndpt"+pa[i],np,pmin,pmax);
      hist3[i].book("dndmt"+pa[i],np,pmin,pmax);
      hist4[i].book("dndxF"+pa[i],nx,xmin,xmax);
      histpt[i].book("<pt> as a function of x_F"+pa[i],nx,xmin,xmax);
    }
}

void MyHist::fill(JAM* jam)
{
    Vec4 ptot=0.0;
    list<EventParticle*>::const_iterator jp;
    list<EventParticle*>&  plist = jam->getEvent();

    //cout << " JAM= " << plist.size() <<endl;

    for(jp=plist.begin(); jp != plist.end(); jp++) {
	int   id  = (*jp)->getID();
	double rap = (*jp)->getP().rap();
	double eta = (*jp)->getP().eta();
	double pt = (*jp)->getP().pT();
	double m = (*jp)->getMass();
	double mt = sqrt(m*m + pt*pt);
	double xf = (*jp)->getP().pz() /pCM;
	int ischarged = (*jp)->charge() != 0 ? 1 : 0;

	if(ischarged) histc[0].fill(eta,1.0/dy);

	ptot += (*jp)->getP();

	int ih=-1;
	double w = 1.0;
	switch (id) {
	  case  2212 : ih=0; break;
	  case -2212 : ih=1;w=-1.0; break;
	  case -211  : ih=2; break;
	  case  211  : ih=3; break;
	  case  111  : ih=4; break;
	  case -321  : ih=5; break;
	  case  321  : ih=6; break;
          case  311  : 
          case -311  : ih=12; break;  // K0
	  case  3122 : ih=7; break; // Lambda
	  case  3112 : ih=9; break; // Sigma-
	  case  3212 : ih=10; break; // Sigma0
	  case  3222 : ih=11; break; // Sigma+
	  default: ih=-1;
	}
		       
	if(ih>=0) {
	  hist1[ih].fill(rap,1.0/dy);
	  hist2[ih].fill(pt,1.0/dp/pt/2.0);
	  //if(abs(rap)<0.1) hist3[ih].fill(mt-m,1.0/dp/mt/0.2);
	  if(rap>0.0 && rap<0.2) hist3[ih].fill(mt-m,1.0/dp/mt/0.2);
	  hist4[ih].fill(xf,1.0/dx);
	  histpt[ih].fill(xf,pt/dx);

	  if(ih==1) {
	    hist1[8].fill(rap,w/dy);
	    hist2[8].fill(pt,w/dp/pt/2.0);
	    //if(abs(rap)<0.1) hist3[8].fill(mt-m,w/dp/mt/0.2);
	    if(rap>0.0 && rap<0.2) hist3[8].fill(mt-m,w/dp/mt/0.2);
	    hist4[8].fill(xf,w/dx);
	    histpt[8].fill(xf,w*pt/dx);
	  }
	}

	// take care of net-proton.
	if(ih==0) {
	  hist1[8].fill(rap,1.0/dy);
	  hist2[8].fill(pt,1.0/dp/pt/2.0);
	  //if(abs(rap)<0.1) hist3[1].fill(mt-m,1.0/dp/mt/0.2);
	  if(rap>0.0 && rap<0.2) hist3[8].fill(mt-m,1.0/dp/mt/0.2);
	  hist4[8].fill(xf,1.0/dx);
	  histpt[8].fill(xf,pt/dx);
	}

    }

    //if(ptot.pAbs()>1e-5) cout << " ptot= " << ptot << " abs= " << ptot.pAbs() <<endl;
}

void MyHist::print(int nev)
{
  string Dir="hist";
  string pa[nhist1]={"p", "pbar", "pi-", "pi+","pi0","K-","K+","Lambda","netp","Sigma-","Sigma0","Sigma+","K0+K0bar"};
  struct stat st;
  if(stat(Dir.c_str(),&st) !=0) mkdir(Dir.c_str(),0775);

  for(int i=0;i<nhist1;i++) {
    Hist *h = new Hist("<pt(x_F)> "+pa[i],nx,xmin,xmax);
    *h = histpt[i] / hist4[i];
    h->table(Dir+"/pt_"+pa[i]+".dat");
    delete h;
  }

    double wei = 1.0/(nev);
    for(int i=0;i<nhist1;i++) {
      hist1[i] *= wei;
      hist2[i] *= wei;
      hist3[i] *= wei;
      hist4[i] *= wei;
      histpt[i] *= wei;
    }
    histc[0] *= wei;
    histc[1] *= wei;
    histc[0].table(Dir+"/dndeta.dat");

    for(int i=0;i<nhist1;i++) {
      hist1[i].table(Dir+"/dndy_"+pa[i]+".dat");
      hist2[i].table(Dir+"/dndpt_"+pa[i]+".dat");
      hist3[i].table(Dir+"/dndmt_"+pa[i]+".dat");
      hist4[i].table(Dir+"/dndx_"+pa[i]+".dat");
    }

}

int npar[4];

void jamee(JAM* jam)
{
    double ecm=4.93;
    Pythia* pythia = jam->getHadronize();
    Event& event = pythia->event;
    event.reset();
    int    id1 = 2;
    int    id2 = 2101;
    double m1 = jam->particleData->m0(id1);
    double m2 = jam->particleData->m0(id2);
    //double pp = sqrtpos(ee*ee - mm*mm);
    double  pp =PCM(ecm,m1,m2);
    double ee1 = sqrt(m1*m1 + pp*pp);
    double ee2 = sqrt(m2*m2 + pp*pp);
    event.append(  id1, 23, 101,   0, 0., 0.,  pp, ee1, m1);
    event.append(  id2, 23,   0, 101, 0., 0., -pp, ee2, m2);
    pythia->next();

    for(int i=0; i<event.size();i++) {
	if(!event[i].isFinal()) continue;
	int   id  = event[i].id();
	//double rap = event[i].p().rap();
	//double pt = event[i].p().pT();
	if(id==2212) {
	    npar[0]++;
	} else if(id==-2212) {
	    npar[1]++;
	} else if(id == - 211) {
	    npar[2]++;
	} else if(id == 211) {
	    npar[3]++;
	}
    }

}

void jamevent() {

    //ofstream ofs("PCM.INFO");

    MyHist* myhist = new MyHist();

    int nev=10;
    JAM *jam = new JAM();
    jam->init();
    nev = jam->mode("Main:numberOfEvents");
    myhist->init(jam->settings->parm("Beams:eCM"));

    for(int iev=1; iev<=nev; iev++) {

	jam->next();
	myhist->fill(jam);

    } //end event loop

    myhist->print(nev);
}

void jamevent_ee()
{
    npar[0]=npar[1]=npar[2]=npar[3]=0;

    int nev=10;

    JAM *jam = new JAM();
    jam->init();
    nev = jam->mode("Main:numberOfEvents");


    for(int iev=1; iev<=nev; iev++) {

	//jam->next();
	jamee(jam);

	//myhist->fill(jam);

    } //end event loop

    //myhist->print(nev);

    double wei=1.0/nev;
    cout << "proton= " << npar[0]*wei << endl;;
    cout << "anti-proton= " << npar[1]*wei << endl;;
    cout << "pi-= " << npar[2]*wei << endl;;
    cout << "pi+= " << npar[3]*wei << endl;;
}


void xsec(string file, string outfile)
{
  JAM *jam = new JAM();
  jam->readFile(file);
  jam->init();
  CrossSection *xsec = jam->xsection;
  JamParticleData* jamParticleData = jam->jamParticleData;
  //SigmaMB *sigmb = xsec->getMB();

  ofstream ofs(outfile.c_str());

  //int id1=111;  // pi0
  //int id1=211;  // pi+
  //int id1=-211;  // pi-
  //int id1=321;  // K+
  //int id1=-321;  // K-
  //int id1=2212; // proton
  int id1=-2212; // anti-proton
  //int id1=-2112; // anti-neutron
  //int id1=3112; // sigma-
  //int id1=3122; // lambda

  //int id2=2212; // proton
  int id2=2112; // neutron
 
  // Specify collision type.
  //int icltype=1; // BB collision.
  //int icltype=2; // MB collision.
  //int icltype=3; // MM collision.
  int icltype=4; // B~B collision.

  ParticleDataEntryPtr pd1 =jamParticleData->find(id1);
  ParticleDataEntryPtr pd2 =jamParticleData->find(id2);
  int pid1=jamParticleData->pid(id1);
  int pid2=jamParticleData->pid(id2);
  EventParticle* pa1 = new EventParticle(id1,pd1);
  EventParticle* pa2 = new EventParticle(id2,pd2);
  pa1->setPID(pid1);
  pa2->setPID(pid2);
  double m1 = pd1->m0();
  double m2 = pd2->m0();
  pa1->setMass(m1);
  pa2->setMass(m2);

  bool islog=false;
  islog=true;
  double smin= m1 + m2 + 0.0;
  double smax= 1000000;
  //double smax= 100;
  int nn=4000;
  double g = 1.0/nn*log10(smax/smin);
  //double ds= (smax - smin)/nn; 
  double ds=0.005;
  if(!islog) nn = int((smax-smin)/ds);

  ofs << "#  m1= " << m1 << " m2= " << m2 <<endl;
  for(int i=1;i<=nn;i++) {
    double srt = islog ? smin*pow(10,i*g) :  m1 + m2 + i*ds;
    double s = srt * srt;
    double pr=sqrt((s-(m1+m2)*(m1+m2))*(s-(m1-m2)*(m1-m2))/(4*s));
    CollisionPair cpair = CollisionPair(icltype,pa1,pa2,srt,pr);

    double sig = xsec->sigma(cpair);
    double sigel = xsec->sigmaEl();

    //double sig = cpair.getSigma();
    //double sigel = cpair.getSigmaElastic();
    double sigelbw = cpair.getElasticBW();
    double sigabs = cpair.getSigAbs();
    double sigch = 0.0;
    if(icltype==4) {
      sigch = cpair.getChargeEx();
    }

    double plab=plabsr(srt,m1,m2);
    ofs << setw(5) << srt << setw(15)<< plab
	 << setw(15) << sig << setw(15) << sigel << setw(15) << sigelbw
	 << setw(15) << sigabs
	 << setw(15) << sigch
	 <<endl;
  }

  ofs.close();
}

int main(int argc, char* argv[]) {

  string outputFname="/Users/feng/Documents/physics_project/test_EG/JAM2_test_storage/test_";
  string inputFname="./playground/jam_";
  string outfile="xsec.dat";
  MyHist* myhist = 0;
  ParticleMult *pmult = new ParticleMult();
  bool hist=false;
  double ymax=5.0;
  bool outputPhaseData=true; // phase space output with gzip file.
  OutPutPhaseSpaceJAM *outputps=0;
  int optV=1; // =1:output last collision point =0:particle coordinate
  int rootOutput=1; // option for root output 
  for(int i=1; i<argc; i++) {
    if(!strcmp(argv[i],"-f")) inputFname = argv[i+1];
    if(!strcmp(argv[i],"-p")) outputPhaseData = atoi(argv[i+1]);
    if(!strcmp(argv[i],"-h")) hist = atoi(argv[i+1]);
    if(!strcmp(argv[i],"-y")) ymax = atof(argv[i+1]);
    if(!strcmp(argv[i],"-o")) outfile = argv[i+1];
    if(!strcmp(argv[i],"-v")) optV= atoi(argv[i+1]);
    if(!strcmp(argv[i],"-r")) rootOutput= atoi(argv[i+1]);
    if(!strcmp(argv[i],"-j")) { outputFname.append(argv[i+1]);
                                outputFname.append(".dat");
                                inputFname.append(argv[i+1]);
                                inputFname.append(".inp");   
                              }
  }

  clock_t start = clock();

  JAM *jam = new JAM();

  // read parameter from a file.
  jam->readFile(inputFname);

  // JAM initial condition 
  InitialCondition* initcnd=0;

  // initial condition from user.
  int ini_opt = jam->settings->mode("Cascade:initialCondition");
  if(ini_opt>=10) {
    initcnd= new MyInitialCondition(ini_opt,&jam->info,jam->jamParticleData);
  }

  // Initialize JAM.
  jam->init(initcnd);

  int nev = jam->mode("Main:numberOfEvents");
  int oversample = jam->getOverSample();
  int massAB = jam->initcond()->massNumberA() + jam->initcond()->massNumberB();

  if(hist) {
    myhist = new MyHist(-ymax,ymax);
    myhist->init(jam->settings->parm("Beams:eCM"));
  }

  if(outputPhaseData) {
    outputps = new OutPutPhaseSpaceJAM(jam,outputFname,nev);
  }

#ifdef USE_ROOT
  TFile* rootFile=0;
  TTree tree("tree", "jam");
  if(rootOutput) {
    // define the output in ROOT format (TTree)
    TString outputRoot(outputFname);
    outputRoot.ReplaceAll("dat", "root");
    rootFile = new TFile(outputRoot, "RECREATE");
    rootFile->cd();
    if(rootOutput==1) treeEvent.branch(tree);
    else tree.Branch("event",&treeEvent);
  }
#endif

  // Loop over all events.
  for(int iev=1; iev<=nev; iev++) {
    jam->next();
    if(hist) myhist->fill(jam);
    pmult->fill(jam);
    if(outputPhaseData) outputps->fill(iev,massAB,jam,optV);
#ifdef USE_ROOT
    if(rootOutput) {
      outputPhaseSpaceRoot(jam,optV);
      tree.Fill();
    }
#endif

  } //end event loop

#ifdef USE_ROOT
  if(rootOutput) {
    tree.Write();
    rootFile->Close();
    delete rootFile;
  }
#endif

  // output information after simulation.
  if(hist) myhist->print(nev*oversample);
  pmult->print(oversample);
  jam->printInfo();
  if(outputPhaseData) outputps->close();

  delete jam;

  date();
  const double time = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
  int isec = time;
  int imin=time/60;
  int ihrs=imin/60;
  imin -= ihrs*60;
  isec -= ihrs*3600+imin*60;
  printf("CPU time %lf[s]\n", time);
  printf("CPU time= %d h %d m %d s\n",ihrs,imin,isec);

  return 0;
}




