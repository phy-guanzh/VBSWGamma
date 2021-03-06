// system include files

#include <iostream>
#include <memory>
#include "TMath.h"
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"  
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Candidate/interface/ShallowCloneCandidate.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CompositeCandidateFwd.h"
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include<algorithm>
#define Pi 3.141593
#include "Math/VectorUtil.h"
#include "TMath.h"
#include <TFormula.h>
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"
struct sortPt
{
   bool operator()(TLorentzVector* s1, TLorentzVector* s2) const
   {
      return s1->Pt() >= s2->Pt();
   }
} mysortPt;
//
// class declaration
//

class PKUTreeMaker : public edm::EDAnalyzer {
public:
  explicit PKUTreeMaker(const edm::ParameterSet&);
  ~PKUTreeMaker();
  //static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
    
    enum PhotonMatchType {UNMATCHED = 0,
        MATCHED_FROM_GUDSCB,
        MATCHED_FROM_PI0,
        MATCHED_FROM_OTHER_SOURCES};
  
private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  virtual void beginRun(const edm::Run&, const edm::EventSetup&) override;
  virtual void endRun(const edm::Run&, const edm::EventSetup&) override;
  virtual void addTypeICorr( edm::Event const & event );
  virtual double getJEC( reco::Candidate::LorentzVector& rawJetP4, const pat::Jet& jet, double& jetCorrEtaMax, std::vector<std::string> jecPayloadNames_ );
  virtual double getJECOffset( reco::Candidate::LorentzVector& rawJetP4, const pat::Jet& jet, double& jetCorrEtaMax, std::vector<std::string> jecPayloadNames_ );
  math::XYZTLorentzVector getNeutrinoP4(double& MetPt, double& MetPhi, TLorentzVector& lep, int lepType);
  bool hasMatchedPromptElectron(const reco::SuperClusterRef &sc, const edm::Handle<edm::View<pat::Electron> > &eleCol,
                                  const edm::Handle<reco::ConversionCollection> &convCol, const math::XYZPoint &beamspot,
                                  float lxyMin=2.0, float probMin=1e-6, unsigned int nHitsBeforeVtxMax=0);
  int matchToTruth(const reco::Photon &pho,
                     const edm::Handle<edm::View<reco::GenParticle>>  &genParticles, bool &ISRPho, double &dR, int &isprompt);

  int matchToTrueLep(double lept_eta,double lept_phi,
                     const edm::Handle<edm::View<reco::GenParticle>>  &genParticles, double &dR, int &ispromptLep);            //////////////////////////////////

  void findFirstNonPhotonMother(const reco::Candidate *particle,
                                  int &ancestorPID, int &ancestorStatus);
  float EAch(float x); 
  float EAnh(float x);
  float EApho(float x);
  std::vector<std::string> offsetCorrLabel_;
  FactorizedJetCorrector* jecOffset_;
  std::vector<std::string> jetCorrLabel_;
  edm::Handle< double >  rho_;
  edm::EDGetTokenT<double> rhoToken_;
  edm::EDGetTokenT<pat::METCollection>  metInputToken_;
  std::vector<edm::EDGetTokenT<pat::METCollection>> mettokens;
  edm::EDGetTokenT<pat::METCollection> metToken_;
  edm::EDGetTokenT<edm::View<pat::Electron> > electronToken_ ;
  edm::EDGetTokenT<edm::View<pat::Photon> > photonToken_;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
  edm::EDGetTokenT<std::vector<reco::Conversion> > conversionsToken_;
  edm::EDGetTokenT<edm::View<pat::Electron> > looseelectronToken_ ; 
  edm::EDGetTokenT<edm::View<pat::Muon> > loosemuonToken_; 

// Filter
  edm::EDGetTokenT<edm::TriggerResults> 		     noiseFilterToken_;
  edm::Handle< edm::TriggerResults> 			     noiseFilterBits_;
  std::string HBHENoiseFilter_Selector_;
  std::string HBHENoiseIsoFilter_Selector_;
  std::string ECALDeadCellNoiseFilter_Selector_;
  std::string GoodVtxNoiseFilter_Selector_;
  std::string EEBadScNoiseFilter_Selector_;
  std::string globalTightHaloFilter_Selector_;
  edm::EDGetTokenT<bool>  badMuon_Selector_;
  edm::EDGetTokenT<bool>  badChargedHadron_Selector_;

  edm::EDGetTokenT<edm::ValueMap<float> > full5x5SigmaIEtaIEtaMapToken_;
  edm::EDGetTokenT<edm::ValueMap<float> > phoChargedIsolationToken_;
  edm::EDGetTokenT<edm::ValueMap<float> > phoNeutralHadronIsolationToken_;
  edm::EDGetTokenT<edm::ValueMap<float> > phoPhotonIsolationToken_;
  EffectiveAreas effAreaChHadrons_;
  EffectiveAreas effAreaNeuHadrons_;
  EffectiveAreas effAreaPhotons_;

  // ----------member data ---------------------------
  TTree* outTree_;

  double MW_;
  int nevent, run, ls;
  int nVtx;
  double triggerWeight, lumiWeight, pileupWeight;
  double theWeight;
double L1prefiring,L1prefiringup,L1prefiringdown;
double pweight[671];
  double  nump=0.;
  double  numm=0.;
  double  npT, npIT;
  int     nBX;
  double ptVlep, yVlep, phiVlep, massVlep, mtVlep, mtVlepnew;
  double ptVlepJEC, yVlepJEC, phiVlepJEC, massVlepJEC, mtVlepJEC, mtVlepJECnew;
  double Mla, Mva;
  double Mla_f, Mva_f;
  double ptlep1, etalep1, philep1,energylep1;
  int  lep, nlooseeles,nloosemus;
  double met, metPhi, j1metPhi, j2metPhi;
  double j1metPhi_f, j2metPhi_f;
  //Met JEC
float  rawPt;
  double METraw_et, METraw_phi, METraw_sumEt;
  double genMET, MET_et, MET_phi, MET_sumEt, MET_corrPx, MET_corrPy;
  double useless;
  // AK4 Jets
  int ak4jet_hf[6],ak4jet_pf[6];
  double ak4jet_pt[6],ak4jet_eta[6],ak4jet_phi[6],ak4jet_e[6];
  double ak4jet_pt_jer[6];
  double ak4jet_csv[6],ak4jet_icsv[6];
  double drjetlep[6], drjetphoton[6];
  //Photon
  double genphoton_pt[6],genphoton_eta[6],genphoton_phi[6];
  double genmuon_pt[6],genmuon_eta[6],genmuon_phi[6];
  double genelectron_pt[6],genelectron_eta[6],genelectron_phi[6];
  double photon_pt[6],photon_eta[6],photon_phi[6],photon_e[6],photonsc_eta[6],photonsc_phi[6];
  bool   photon_pev[6],photon_pevnew[6],photon_ppsv[6],photon_iseb[6],photon_isee[6];
  double photon_hoe[6],photon_sieie[6],photon_sieie2[6], photon_chiso[6],photon_nhiso[6],photon_phoiso[6],photon_drla[6],photon_mla[6],photon_mva[6];
  int      photon_istrue[6], photon_isprompt[6];
  double photonet, photoneta, photonphi, photone, photonsceta, photonscphi;
  double photonet_f, photoneta_f, photonphi_f, photone_f, photonsceta_f, photonscphi_f;
  double photonsieie, photonphoiso, photonchiso, photonnhiso;
  double photonsieie_f, photonphoiso_f, photonchiso_f, photonnhiso_f;
  int iphoton;
  int iphoton_f;
  double drla, drla_f;
  bool passEleVeto, passEleVetonew, passPixelSeedVeto, photonhaspixelseed, photonhaspixelseed_f,photonpasseleveto,photonpasseleveto_f;
  //Photon gen match
  int   isTrue_;
  bool ISRPho;
  int isprompt_,ispromptLep_,lepton_istrue;    //////////////////////////////////
  double dR_,dR1_;
  //Jets
  int jet1hf,jet1pf,jet2hf,jet2pf,jet1hf_f,jet1pf_f,jet2hf_f,jet2pf_f;
  double jet1pt, jet1eta, jet1phi, jet1e, jet1csv, jet1icsv;
  double jet1pt_f, jet1eta_f, jet1phi_f, jet1e_f, jet1csv_f, jet1icsv_f;
  double jet2pt, jet2eta, jet2phi, jet2e, jet2csv, jet2icsv;
  double jet2pt_f, jet2eta_f, jet2phi_f, jet2e_f, jet2csv_f, jet2icsv_f;
  double drj1a, drj2a, drj1l, drj2l;
  double drj1a_f, drj2a_f, drj1l_f, drj2l_f;
  double Mjj, deltaeta, zepp;
  double Mjj_f, deltaeta_f, zepp_f; 
double Dphiwajj,Dphiwajj_f;

  void setDummyValues();
    

//// L1 prefiring

edm::EDGetTokenT< double > prefweight_token;
edm::EDGetTokenT< double > prefweightup_token;
edm::EDGetTokenT< double > prefweightdown_token;


  /// Parameters to steer the treeDumper
  int originalNEvents_;
  double crossSectionPb_;
  double targetLumiInvPb_;
  std::string PKUChannel_;
  bool isGen_ , RunOnMC_;
  std::vector<std::string> jecAK4Labels_;
  std::vector<std::string> jecAK4chsLabels_;
  //correction jet
  FactorizedJetCorrector* jecAK4_;
  std::string gravitonSrc_;
  std::map<std::string,double>  TypeICorrMap_;
  edm::InputTag mets_;

  //High Level Trigger
  HLTConfigProvider hltConfig;
  edm::EDGetTokenT<edm::TriggerResults> hltToken_;
  std::vector<std::string> elPaths1_, elPaths2_;
  std::vector<std::string> muPaths1_, muPaths2_, muPaths3_;
  std::vector<std::string> elPaths1, elPaths2;
  std::vector<std::string> muPaths1, muPaths2, muPaths3;
  int  HLT_Ele1, HLT_Ele2;
  int  HLT_Mu1, HLT_Mu2, HLT_Mu3;

// filter
  bool passFilter_HBHE_                   ;
  bool passFilter_HBHEIso_                ;
  bool passFilter_globalTightHalo_	  ;
  bool passFilter_ECALDeadCell_           ;
  bool passFilter_GoodVtx_                ;
  bool passFilter_EEBadSc_                ;
  bool passFilter_badMuon_                ;
  bool passFilter_badChargedHadron_       ;

edm::EDGetTokenT<LHEEventProduct> LheToken_;
  edm::EDGetTokenT<GenEventInfoProduct> GenToken_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo>> PUToken_;
  edm::EDGetTokenT<edm::View<reco::Candidate>> leptonicVSrc_;
  edm::EDGetTokenT<edm::View<pat::Jet>> ak4jetsSrc_;
  edm::EDGetTokenT<edm::View<pat::Photon>> photonSrc_;
  edm::EDGetTokenT<edm::View<reco::GenParticle>> genSrc_;
  edm::EDGetTokenT<edm::View<reco::Candidate>> metSrc_;
  edm::EDGetTokenT<reco::VertexCollection> VertexToken_;
  edm::EDGetTokenT<pat::JetCollection> t1jetSrc_;
  edm::EDGetTokenT<edm::View<pat::Muon>> t1muSrc_;
};

float PKUTreeMaker::EAch( float x){
 float EA = 0.0360;
 if(x>1.0)   EA = 0.0377;
 if(x>1.479) EA = 0.0306;
 if(x>2.0)   EA = 0.0283;
 if(x>2.2)   EA = 0.0254;
 if(x>2.3)   EA = 0.0217;
 if(x>2.4)   EA = 0.0167;
 return EA;
}

float PKUTreeMaker::EAnh( float x){
 float EA = 0.0597;
 if(x>1.0)   EA = 0.0807;
 if(x>1.479) EA = 0.0629;
 if(x>2.0)   EA = 0.0197;
 if(x>2.2)   EA = 0.0184;
 if(x>2.3)   EA = 0.0284;
 if(x>2.4)   EA = 0.0591;
 return EA;
}

float PKUTreeMaker::EApho( float x){
 float EA = 0.1210;
 if(x>1.0)   EA = 0.1107;
 if(x>1.479) EA = 0.0699;
 if(x>2.0)   EA = 0.1056;
 if(x>2.2)   EA = 0.1457;
 if(x>2.3)   EA = 0.1719;
 if(x>2.4)   EA = 0.1998;
 return EA;
}


//
// constructors and destructor
//
PKUTreeMaker::PKUTreeMaker(const edm::ParameterSet& iConfig)//:
  :effAreaChHadrons_((iConfig.getParameter<edm::FileInPath>("effAreaChHadFile")).fullPath() )
  ,effAreaNeuHadrons_((iConfig.getParameter<edm::FileInPath>("effAreaNeuHadFile")).fullPath() )
  ,effAreaPhotons_((iConfig.getParameter<edm::FileInPath>("effAreaPhoFile")).fullPath() )
{
  hltToken_=consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("hltToken"));
  elPaths1_=iConfig.getParameter<std::vector<std::string>>("elPaths1");
  elPaths2_=iConfig.getParameter<std::vector<std::string>>("elPaths2");
  muPaths1_=iConfig.getParameter<std::vector<std::string>>("muPaths1");
  muPaths2_=iConfig.getParameter<std::vector<std::string>>("muPaths2");
  muPaths3_=iConfig.getParameter<std::vector<std::string>>("muPaths3");
  GenToken_=consumes<GenEventInfoProduct> (iConfig.getParameter<edm::InputTag>( "generator") ) ;
  LheToken_=consumes<LHEEventProduct> (iConfig.getParameter<edm::InputTag>( "lhe") ) ;
  PUToken_=consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("pileup") ) ;
  leptonicVSrc_=consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>( "leptonicVSrc") ) ;
  ak4jetsSrc_      = consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>( "ak4jetsSrc") ) ;
  photonSrc_      = consumes<edm::View<pat::Photon>>(iConfig.getParameter<edm::InputTag>( "photonSrc") ) ;
  genSrc_      = consumes<edm::View<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>( "genSrc") ) ;
  metSrc_      = consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>( "metSrc") ) ;
  VertexToken_ =consumes<reco::VertexCollection> (iConfig.getParameter<edm::InputTag>( "vertex" ) ) ;
  t1jetSrc_      = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>( "t1jetSrc") ) ;
  t1muSrc_      = consumes<edm::View<pat::Muon>>(iConfig.getParameter<edm::InputTag>( "t1muSrc") ) ;
  originalNEvents_ = iConfig.getParameter<int>("originalNEvents");
  crossSectionPb_  = iConfig.getParameter<double>("crossSectionPb");
  targetLumiInvPb_ = iConfig.getParameter<double>("targetLumiInvPb");
  PKUChannel_     = iConfig.getParameter<std::string>("PKUChannel");
  isGen_           = iConfig.getParameter<bool>("isGen");
  RunOnMC_           = iConfig.getParameter<bool>("RunOnMC");
  rhoToken_  = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
  jecAK4chsLabels_   =  iConfig.getParameter<std::vector<std::string>>("jecAK4chsPayloadNames");
  jecAK4Labels_   =  iConfig.getParameter<std::vector<std::string>>("jecAK4PayloadNames");
  metToken_ = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("metSrc"));
  mettokens.push_back( metToken_ );
  metInputToken_ = mettokens[0];
  electronToken_    = (consumes<edm::View<pat::Electron> > (iConfig.getParameter<edm::InputTag>("electrons")))            ;
  looseelectronToken_    = (consumes<edm::View<pat::Electron> > (iConfig.getParameter<edm::InputTag>("looseelectronSrc"))) ;
  loosemuonToken_    = (consumes<edm::View<pat::Muon> > (iConfig.getParameter<edm::InputTag>("loosemuonSrc")))              ;
  beamSpotToken_    = (consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"))) ;
  conversionsToken_ = (consumes<std::vector<reco::Conversion> >(iConfig.getParameter<edm::InputTag>("conversions"))) ;
    
  jetCorrLabel_ = jecAK4chsLabels_;
  offsetCorrLabel_.push_back(jetCorrLabel_[0]);

//  L1 prefiring
  prefweight_token = consumes< double >(edm::InputTag("prefiringweight:NonPrefiringProb"));
  prefweightup_token = consumes< double >(edm::InputTag("prefiringweight:NonPrefiringProbUp"));
  prefweightdown_token = consumes< double >(edm::InputTag("prefiringweight:NonPrefiringProbDown"));



// filter
   noiseFilterToken_ = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("noiseFilter"));
   HBHENoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_HBHENoiseFilter");
   HBHENoiseIsoFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_HBHENoiseIsoFilter");  
   globalTightHaloFilter_Selector_ = iConfig.getParameter<std::string> ("noiseFilterSelection_globalTightHaloFilter"); 
   ECALDeadCellNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_EcalDeadCellTriggerPrimitiveFilter");
   GoodVtxNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_goodVertices");
   EEBadScNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_eeBadScFilter");
   badMuon_Selector_ =  consumes<bool>(iConfig.getParameter<edm::InputTag> ("noiseFilterSelection_badMuon"));
   badChargedHadron_Selector_ =  consumes<bool>(iConfig.getParameter<edm::InputTag> ("noiseFilterSelection_badChargedHadron"));

   full5x5SigmaIEtaIEtaMapToken_=(consumes <edm::ValueMap<float> >
				(iConfig.getParameter<edm::InputTag>("full5x5SigmaIEtaIEtaMap")));
   phoChargedIsolationToken_=(consumes <edm::ValueMap<float> >
     			        (iConfig.getParameter<edm::InputTag>("phoChargedIsolation")));
   phoNeutralHadronIsolationToken_=(consumes <edm::ValueMap<float> >
                                (iConfig.getParameter<edm::InputTag>("phoNeutralHadronIsolation")));
   phoPhotonIsolationToken_=(consumes <edm::ValueMap<float> >
			        (iConfig.getParameter<edm::InputTag>("phoPhotonIsolation")));

   MW_=80.385; 
  //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  outTree_ = fs->make<TTree>("PKUCandidates","PKU Candidates");
  /// Basic event quantities
  outTree_->Branch("rawPt"           ,&rawPt          ,"rawPt/F"          );
  outTree_->Branch("nevent"          ,&nevent         ,"nevent/I"         );
  outTree_->Branch("run"             ,&run            ,"run/I"            );
  outTree_->Branch("ls"              ,&ls             ,"ls/I"             );
outTree_->Branch("nVtx"            ,&nVtx           ,"nVtx/I"           );
  outTree_->Branch("theWeight"           ,&theWeight         ,"theWeight/D"          );
  outTree_->Branch("L1prefiring"           ,&L1prefiring         ,"L1prefiring/D"          );
  outTree_->Branch("L1prefiringup"           ,&L1prefiringup         ,"L1prefiringup/D"          );
  outTree_->Branch("L1prefiringdown"           ,&L1prefiringdown         ,"L1prefiringdown/D"          );
outTree_->Branch("pweight"           ,pweight         ,"pweight[671]/D"          );
  outTree_->Branch("nump"           ,&nump         ,"nump/D"          );
  outTree_->Branch("numm"           ,&numm         ,"numm/D"          );
  outTree_->Branch("npT"           ,&npT         ,"npT/D"          );
  outTree_->Branch("lep"             ,&lep            ,"lep/I"            );
  outTree_->Branch("ptVlep"          ,&ptVlep         ,"ptVlep/D"         );
  outTree_->Branch("ptVlepJEC"          ,&ptVlepJEC         ,"ptVlepJEC/D"         );
  outTree_->Branch("yVlepJEC"           ,&yVlepJEC          ,"yVlepJEC/D"          );
  outTree_->Branch("phiVlepJEC"         ,&phiVlepJEC        ,"phiVlepJEC/D"        );
  outTree_->Branch("massVlepJEC"        ,&massVlepJEC       ,"massVlepJEC/D"       );
  outTree_->Branch("mtVlepJECnew"          ,&mtVlepJECnew         ,"mtVlepJECnew/D"         );
  outTree_->Branch("Mla"          ,&Mla         ,"Mla/D"         );
  outTree_->Branch("Mla_f"          ,&Mla_f         ,"Mla_f/D"         );
  outTree_->Branch("Mva"          ,&Mva         ,"Mva/D"         );
  outTree_->Branch("Mva_f"          ,&Mva_f         ,"Mva_f/D"         );
  outTree_->Branch("nlooseeles"          ,&nlooseeles         ,"nlooseeles/I"         );
  outTree_->Branch("nloosemus"          ,&nloosemus         ,"nloosemus/I"         );
/*
  outTree_->Branch("genphoton_pt"        , genphoton_pt       ,"genphoton_pt[6]/D"       );
  outTree_->Branch("genphoton_eta"        , genphoton_eta       ,"genphoton_eta[6]/D"       );
  outTree_->Branch("genphoton_phi"        , genphoton_phi       ,"genphoton_phi[6]/D"       );
  outTree_->Branch("genmuon_pt"        , genmuon_pt       ,"genmuon_pt[6]/D"       );
  outTree_->Branch("genmuon_eta"        , genmuon_eta       ,"genmuon_eta[6]/D"       );
  outTree_->Branch("genmuon_phi"        , genmuon_phi       ,"genmuon_phi[6]/D"       );
  outTree_->Branch("genelectron_pt"        , genelectron_pt       ,"genelectron_pt[6]/D"       );
  outTree_->Branch("genelectron_eta"        , genelectron_eta       ,"genelectron_eta[6]/D"       );
  outTree_->Branch("genelectron_phi"        , genelectron_phi       ,"genelectron_phi[6]/D"       );
*/
  /// Photon
  outTree_->Branch("photon_pt"        , photon_pt       ,"photon_pt[6]/D"       );
  outTree_->Branch("photon_eta"        , photon_eta       ,"photon_eta[6]/D"       );
  outTree_->Branch("photon_phi"        , photon_phi       ,"photon_phi[6]/D"       );
  outTree_->Branch("photon_e"        , photon_e       ,"photon_e[6]/D"       );
  outTree_->Branch("photonsc_eta"        , photonsc_eta       ,"photonsc_eta[6]/D"       );
  outTree_->Branch("photonsc_phi"        , photonsc_phi       ,"photonsc_phi[6]/D"       );
  outTree_->Branch("photon_pev"        , photon_pev       ,"photon_pev[6]/O"       );
  outTree_->Branch("photon_pevnew"        , photon_pevnew       ,"photon_pevnew[6]/O"       );
  outTree_->Branch("photon_ppsv"        , photon_ppsv       ,"photon_ppsv[6]/O"       );
//  outTree_->Branch("photon_iseb"        , photon_iseb       ,"photon_iseb[6]/O"       );
//  outTree_->Branch("photon_isee"        , photon_isee       ,"photon_isee[6]/O"       );
  outTree_->Branch("photon_hoe"        , photon_hoe       ,"photon_hoe[6]/D"       );
  outTree_->Branch("photon_sieie"        , photon_sieie       ,"photon_sieie[6]/D"       );
  outTree_->Branch("photon_sieie2"        , photon_sieie2       ,"photon_sieie2[6]/D"       );
  outTree_->Branch("photon_chiso"        , photon_chiso       ,"photon_chiso[6]/D"       );
  outTree_->Branch("photon_nhiso"        , photon_nhiso       ,"photon_nhiso[6]/D"       );
  outTree_->Branch("photon_phoiso"        , photon_phoiso       ,"photon_phoiso[6]/D"       );
//  outTree_->Branch("photon_istrue"        , photon_istrue       ,"photon_istrue[6]/I"       );
  outTree_->Branch("photon_isprompt"        , photon_isprompt       ,"photon_isprompt[6]/I"       );
  outTree_->Branch("photon_drla"        , photon_drla       ,"photon_drla[6]/D"       );
  outTree_->Branch("photon_mla"        , photon_mla       ,"photon_mla[6]/D"       );
//  outTree_->Branch("photon_mva"        , photon_mva       ,"photon_mva[6]/D"       );
//  outTree_->Branch("passEleVeto"        , &passEleVeto       ,"passEleVeto/O"       );
//  outTree_->Branch("passEleVetonew"        , &passEleVetonew       ,"passEleVetonew/O"       );
//  outTree_->Branch("passPixelSeedVeto"        , &passPixelSeedVeto       ,"passPixelSeedVeto/O"       );
  outTree_->Branch("photonhaspixelseed"        , &photonhaspixelseed       ,"photonhaspixelseed/O"       );
  outTree_->Branch("photonhaspixelseed_f"        , &photonhaspixelseed_f      ,"photonhaspixelseed_f/O"       );
  outTree_->Branch("photonpasseleveto"        , &photonpasseleveto       ,"photonpasseleveto/O"       );
  outTree_->Branch("photonpasseleveto_f"        , &photonpasseleveto_f       ,"photonpasseleveto_F/O"       );
  outTree_->Branch("photonet"          ,&photonet         ,"photonet/D"         );
  outTree_->Branch("photonet_f"          ,&photonet_f         ,"photonet_f/D"         );
  outTree_->Branch("photoneta"          ,&photoneta         ,"photoneta/D"         );
  outTree_->Branch("photoneta_f"          ,&photoneta_f         ,"photoneta_f/D"         );
  outTree_->Branch("photonphi"          ,&photonphi         ,"photonphi/D"         );
  outTree_->Branch("photonphi_f"          ,&photonphi_f         ,"photonphi_f/D"         );
  outTree_->Branch("photone"          ,&photone         ,"photone/D"         );
  outTree_->Branch("photone_f"          ,&photone_f         ,"photone_f/D"         );
  outTree_->Branch("photonsceta"          ,&photonsceta         ,"photonsceta/D"         );
  outTree_->Branch("photonsceta_f"          ,&photonsceta_f         ,"photonsceta_f/D"         );
  outTree_->Branch("photonscphi"          ,&photonscphi         ,"photonscphi/D"         );
  outTree_->Branch("photonscphi_f"          ,&photonscphi_f         ,"photonscphi_f/D"         );
/*
  outTree_->Branch("photonsieie"          ,&photonsieie         ,"photonsieie/D"         );
  outTree_->Branch("photonsieie_f"          ,&photonsieie_f         ,"photonsieie_f/D"         );
  outTree_->Branch("photonphoiso"          ,&photonphoiso         ,"photonphoiso/D"         );
  outTree_->Branch("photonphoiso_f"          ,&photonphoiso_f         ,"photonphoiso_f/D"         );
  outTree_->Branch("photonchiso"          ,&photonchiso         ,"photonchiso/D"         );
  outTree_->Branch("photonchiso_f"          ,&photonchiso_f         ,"photonchiso_f/D"         );
  outTree_->Branch("photonnhiso"          ,&photonnhiso         ,"photonnhiso/D"         );
  outTree_->Branch("photonnhiso_f"          ,&photonnhiso_f         ,"photonnhiso_f/D"         );
*/
  outTree_->Branch("iphoton"             ,&iphoton            ,"iphoton/I"            );
  outTree_->Branch("iphoton_f"             ,&iphoton_f            ,"iphoton_f/I"            );
  outTree_->Branch("drla"          ,&drla         ,"drla/D"         );
  outTree_->Branch("drla_f"          ,&drla_f         ,"drla_f/D"         );
    //photon gen match
//    outTree_->Branch("dR"    , &dR_, "dR/D");
//    outTree_->Branch("ISRPho"        , &ISRPho       ,"ISRPho/O"       );
  outTree_->Branch("isTrue", &isTrue_, "isTrue/I");
  outTree_->Branch("isprompt"    , &isprompt_, "isprompt/I");
  outTree_->Branch("ispromptLep"    , &ispromptLep_, "ispromptLep/I");
//jets
  outTree_->Branch("ak4jet_hf"        ,ak4jet_hf       ,"ak4jet_hf[6]/I"       );
  outTree_->Branch("ak4jet_pf"        ,ak4jet_pf       ,"ak4jet_pf[6]/I"       );
  outTree_->Branch("jet1hf"        	,&jet1hf     ,"jet1hf/I"       );
  outTree_->Branch("jet1pf"        	,&jet1pf     ,"jet1pf/I"       );
  outTree_->Branch("jet2hf"        	,&jet2hf     ,"jet2hf/I"       );
  outTree_->Branch("jet2pf"        	,&jet2pf     ,"jet2pf/I"       );
  outTree_->Branch("jet1hf_f"        ,&jet1hf_f       ,"jet1hf_f/I"       );
  outTree_->Branch("jet1pf_f"        ,&jet1pf_f       ,"jet1pf_f/I"       );
  outTree_->Branch("jet2hf_f"        ,&jet2hf_f       ,"jet2hf_f/I"       );
  outTree_->Branch("jet2pf_f"        ,&jet2pf_f       ,"jet2pf_f/I"       );

  outTree_->Branch("ak4jet_pt"        , ak4jet_pt       ,"ak4jet_pt[6]/D"       );
  outTree_->Branch("ak4jet_eta"        , ak4jet_eta       ,"ak4jet_eta[6]/D"       );
  outTree_->Branch("ak4jet_phi"        , ak4jet_phi       ,"ak4jet_phi[6]/D"       );
  outTree_->Branch("ak4jet_e"        , ak4jet_e       ,"ak4jet_e[6]/D"       );
  outTree_->Branch("ak4jet_csv"        , ak4jet_csv       ,"ak4jet_csv[6]/D"       );
  outTree_->Branch("ak4jet_icsv"        , ak4jet_icsv       ,"ak4jet_icsv[6]/D"       );
  outTree_->Branch("jet1pt"          ,&jet1pt         ,"jet1pt/D"         );
  outTree_->Branch("jet1pt_f"          ,&jet1pt_f         ,"jet1pt_f/D"         );
  outTree_->Branch("jet1eta"          ,&jet1eta         ,"jet1eta/D"         );
  outTree_->Branch("jet1eta_f"          ,&jet1eta_f         ,"jet1eta_f/D"         );
  outTree_->Branch("jet1phi"          ,&jet1phi         ,"jet1phi/D"         );
  outTree_->Branch("jet1phi_f"          ,&jet1phi_f         ,"jet1phi_f/D"         );
  outTree_->Branch("jet1e"          ,&jet1e         ,"jet1e/D"         );
  outTree_->Branch("jet1e_f"          ,&jet1e_f         ,"jet1e_f/D"         );
  outTree_->Branch("jet1csv"          ,&jet1csv         ,"jet1csv/D"         );    //Not used
  outTree_->Branch("jet1csv_f"          ,&jet1csv_f         ,"jet1csv_f/D"         );    //Not used
  outTree_->Branch("jet1icsv"          ,&jet1icsv         ,"jet1icsv/D"         ); //Not used
  outTree_->Branch("jet1icsv_f"          ,&jet1icsv_f         ,"jet1icsv_f/D"         ); //Not used
  outTree_->Branch("jet2pt"          ,&jet2pt         ,"jet2pt/D"         );
  outTree_->Branch("jet2pt_f"          ,&jet2pt_f         ,"jet2pt_f/D"         );
  outTree_->Branch("jet2eta"          ,&jet2eta         ,"jet2eta/D"         );
  outTree_->Branch("jet2eta_f"          ,&jet2eta_f         ,"jet2eta_f/D"         );
  outTree_->Branch("jet2phi"          ,&jet2phi         ,"jet2phi/D"         );
  outTree_->Branch("jet2phi_f"          ,&jet2phi_f         ,"jet2phi_f/D"         );
  outTree_->Branch("jet2e"          ,&jet2e         ,"jet2e/D"         );
  outTree_->Branch("jet2e_f"          ,&jet2e_f         ,"jet2e_f/D"         );
  outTree_->Branch("jet2csv"          ,&jet2csv         ,"jet2csv/D"         );
  outTree_->Branch("jet2csv_f"          ,&jet2csv_f         ,"jet2csv_f/D"         );
  outTree_->Branch("jet2icsv"          ,&jet2icsv         ,"jet2icsv/D"         );
  outTree_->Branch("jet2icsv_f"          ,&jet2icsv_f         ,"jet2icsv_f/D"         );
  outTree_->Branch("drj1a"          ,&drj1a         ,"drj1a/D"         );
  outTree_->Branch("drj1a_f"          ,&drj1a_f         ,"drj1a_f/D"         );
  outTree_->Branch("drj2a"          ,&drj2a         ,"drj2a/D"         );
  outTree_->Branch("drj2a_f"          ,&drj2a_f         ,"drj2a_f/D"         );
  outTree_->Branch("drj1l"          ,&drj1l         ,"drj1l/D"         );
  outTree_->Branch("drj1l_f"          ,&drj1l_f         ,"drj1l_f/D"         );
  outTree_->Branch("drj2l"          ,&drj2l         ,"drj2l/D"         );
  outTree_->Branch("drj2l_f"          ,&drj2l_f         ,"drj2l_f/D"         );
  outTree_->Branch("Mjj"          ,&Mjj         ,"Mjj/D"         );
  outTree_->Branch("Mjj_f"          ,&Mjj_f         ,"Mjj_f/D"         );
  outTree_->Branch("deltaeta"          ,&deltaeta         ,"deltaeta/D"         );
  outTree_->Branch("deltaeta_f"          ,&deltaeta_f         ,"deltaeta_f/D"         );
  outTree_->Branch("zepp"          ,&zepp         ,"zepp/D"         );
  outTree_->Branch("zepp_f"          ,&zepp_f         ,"zepp_f/D"         );
  // Generic kinematic quantities
  outTree_->Branch("ptlep1"          ,&ptlep1         ,"ptlep1/D"         );
  outTree_->Branch("etalep1"         ,&etalep1        ,"etalep1/D"        );
  outTree_->Branch("philep1"         ,&philep1        ,"philep1/D"        );
  outTree_->Branch("energylep1"         ,&energylep1        ,"energylep1/D"        );
  outTree_->Branch("j1metPhi"          ,&j1metPhi         ,"j1metPhi/D"         );
  outTree_->Branch("j1metPhi_f"          ,&j1metPhi_f         ,"j1metPhi_f/D"         );
  outTree_->Branch("j2metPhi"          ,&j2metPhi         ,"j2metPhi/D"         );
  outTree_->Branch("j2metPhi_f"          ,&j2metPhi_f         ,"j2metPhi_f/D"         );
  outTree_->Branch("Dphiwajj"		,&Dphiwajj	   ,"Dphiwajj/D"	);
  outTree_->Branch("Dphiwajj_f"		,&Dphiwajj_f	   ,"Dphiwajj_f/D"	);

  // MET   
  outTree_->Branch("METraw_et",&METraw_et,"METraw_et/D");
  outTree_->Branch("METraw_phi",&METraw_phi,"METraw_phi/D");
  outTree_->Branch("METraw_sumEt",&METraw_sumEt,"METraw_sumEt/D");
  outTree_->Branch("genMET",&genMET,"genMET/D");
  outTree_->Branch("MET_et",&MET_et,"MET_et/D");
  outTree_->Branch("MET_phi",&MET_phi,"MET_phi/D");
  outTree_->Branch("MET_sumEt",&MET_sumEt,"MET_sumEt/D");
  outTree_->Branch("MET_corrPx",&MET_corrPx,"MET_corrPx/D");
  outTree_->Branch("MET_corrPy",&MET_corrPy,"MET_corrPy/D");
  //HLT bits
  outTree_->Branch("HLT_Ele1"  ,&HLT_Ele1 ,"HLT_Ele1/I" );
  outTree_->Branch("HLT_Ele2"  ,&HLT_Ele2 ,"HLT_Ele2/I" );
  outTree_->Branch("HLT_Mu1"   ,&HLT_Mu1  ,"HLT_Mu1/I"  );
  outTree_->Branch("HLT_Mu2"   ,&HLT_Mu2  ,"HLT_Mu2/I"  );
  outTree_->Branch("HLT_Mu3"   ,&HLT_Mu3  ,"HLT_Mu3/I"  );
  // filter
/*
  outTree_->Branch("passFilter_HBHE"                 ,&passFilter_HBHE_                ,"passFilter_HBHE_/O");
  outTree_->Branch("passFilter_HBHEIso"                 ,&passFilter_HBHEIso_                ,"passFilter_HBHEIso_/O");
  outTree_->Branch("passFilter_globalTightHalo"		,&passFilter_globalTightHalo_    ,"passFilter_globalTightHalo_/O");
  outTree_->Branch("passFilter_ECALDeadCell"         ,&passFilter_ECALDeadCell_        ,"passFilter_ECALDeadCell_/O");
  outTree_->Branch("passFilter_GoodVtx"              ,&passFilter_GoodVtx_             ,"passFilter_GoodVtx_/O");
  outTree_->Branch("passFilter_EEBadSc"              ,&passFilter_EEBadSc_             ,"passFilter_EEBadSc_/O");
  outTree_->Branch("passFilter_badMuon"                 ,&passFilter_badMuon_                ,"passFilter_badMuon_/O");
  outTree_->Branch("passFilter_badChargedHadron"                 ,&passFilter_badChargedHadron_                ,"passFilter_badChargedHadron_/O");
*/
//  outTree_->Branch("triggerWeight"   ,&triggerWeight  ,"triggerWeight/D"  );
  outTree_->Branch("lumiWeight"      ,&lumiWeight     ,"lumiWeight/D"     );
  outTree_->Branch("pileupWeight"    ,&pileupWeight   ,"pileupWeight/D"   );
}

//------------------------------------
double PKUTreeMaker::getJEC( reco::Candidate::LorentzVector& rawJetP4, const pat::Jet& jet, double& jetCorrEtaMax, std::vector<std::string> jecPayloadNames_ ){
    double jetCorrFactor = 1.;
    if ( fabs(rawJetP4.eta()) < jetCorrEtaMax ){
        jecAK4_->setJetEta( rawJetP4.eta() );
        jecAK4_->setJetPt ( rawJetP4.pt() );
        jecAK4_->setJetE  ( rawJetP4.energy() );
        jecAK4_->setJetPhi( rawJetP4.phi()    );
        jecAK4_->setJetA  ( jet.jetArea() );
        jecAK4_->setRho   ( *(rho_.product()) );
        jecAK4_->setNPV   ( nVtx );
        jetCorrFactor = jecAK4_->getCorrection();
    }
    reco::Candidate::LorentzVector corrJetP4 = rawJetP4;
    corrJetP4 *= jetCorrFactor;
    return jetCorrFactor;
}
//------------------------------------
double PKUTreeMaker::getJECOffset( reco::Candidate::LorentzVector& rawJetP4, const pat::Jet& jet, double& jetCorrEtaMax, std::vector<std::string> jecPayloadNames_ ){
    double jetCorrFactor = 1.;
    if ( fabs(rawJetP4.eta()) < jetCorrEtaMax ){
        jecOffset_->setJetEta( rawJetP4.eta()     );
        jecOffset_->setJetPt ( rawJetP4.pt()      );
        jecOffset_->setJetE  ( rawJetP4.energy()  );
        jecOffset_->setJetPhi( rawJetP4.phi()     );
        jecOffset_->setJetA  ( jet.jetArea()      );
        jecOffset_->setRho   ( *(rho_.product())  );
        jecOffset_->setNPV   ( nVtx  );
        jetCorrFactor = jecOffset_->getCorrection();
    }
    reco::Candidate::LorentzVector corrJetP4 = rawJetP4;
    corrJetP4 *= jetCorrFactor;
    return jetCorrFactor;
}
//------------------------------------
void PKUTreeMaker::addTypeICorr( edm::Event const & event ){
    TypeICorrMap_.clear();
    edm::Handle<pat::JetCollection> jets_;
    event.getByToken(t1jetSrc_, jets_);
    event.getByToken(rhoToken_      , rho_     );
    edm::Handle<reco::VertexCollection> vertices_;
    event.getByToken(VertexToken_, vertices_);
    edm::Handle<edm::View<pat::Muon>> muons_;
    event.getByToken(t1muSrc_,muons_);
    bool skipEM_                    = true;
    double skipEMfractionThreshold_ = 0.9;
    bool skipMuons_                 = true;
    std::string skipMuonSelection_string = "isGlobalMuon | isStandAloneMuon";
    StringCutObjectSelector<reco::Candidate>* skipMuonSelection_ = new StringCutObjectSelector<reco::Candidate>(skipMuonSelection_string,true);
    double jetCorrEtaMax_           = 9.9;
    double type1JetPtThreshold_     = 15.0;
    double corrEx    = 0;
    double corrEy    = 0;
    double corrSumEt = 0;

    std::vector<JetCorrectorParameters> vPar;
    for ( std::vector<std::string>::const_iterator payloadBegin = jecAK4chsLabels_.begin(), payloadEnd = jecAK4chsLabels_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
        JetCorrectorParameters pars(*ipayload);
        vPar.push_back(pars);
    }
    jecAK4_ = new FactorizedJetCorrector(vPar);
    vPar.clear();
    for ( std::vector<std::string>::const_iterator payloadBegin = offsetCorrLabel_.begin(), payloadEnd = offsetCorrLabel_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
        JetCorrectorParameters pars(*ipayload);
        vPar.push_back(pars);
    }
    jecOffset_ = new FactorizedJetCorrector(vPar);
    vPar.clear();

    for (const pat::Jet &jet : *jets_) {
        double emEnergyFraction = jet.chargedEmEnergyFraction() + jet.neutralEmEnergyFraction();
        if ( skipEM_ && emEnergyFraction > skipEMfractionThreshold_ ) continue;
        reco::Candidate::LorentzVector rawJetP4 = jet.correctedP4(0);
        double corr = getJEC(rawJetP4, jet, jetCorrEtaMax_, jetCorrLabel_);   
/*
        if ( skipMuons_ && jet.muonMultiplicity() != 0 ) {
            for (const pat::Muon &muon : *muons_) {
                if( !muon.isGlobalMuon() && !muon.isStandAloneMuon() ) continue;
                TLorentzVector muonV; muonV.SetPtEtaPhiE(muon.p4().pt(),muon.p4().eta(),muon.p4().phi(),muon.p4().e());
                TLorentzVector jetV; jetV.SetPtEtaPhiE(jet.p4().pt(),jet.p4().eta(),jet.p4().phi(),jet.p4().e());
                if( muonV.DeltaR(jetV) < 0.5 ){
                    reco::Candidate::LorentzVector muonP4 = muon.p4();
                    rawJetP4 -= muonP4;
                }
            }
        }
*/
     if ( skipMuons_ ) {
       const std::vector<reco::CandidatePtr> & cands = jet.daughterPtrVector();
       for ( std::vector<reco::CandidatePtr>::const_iterator cand = cands.begin();
             cand != cands.end(); ++cand ) {
     	 const reco::PFCandidate *pfcand = dynamic_cast<const reco::PFCandidate *>(cand->get());
     	 const reco::Candidate *mu = (pfcand != 0 ? ( pfcand->muonRef().isNonnull() ? pfcand->muonRef().get() : 0) : cand->get());
         if ( mu != 0 && (*skipMuonSelection_)(*mu) ) {
           reco::Candidate::LorentzVector muonP4 = (*cand)->p4();
           rawJetP4 -= muonP4;
         }
       }
     }
        reco::Candidate::LorentzVector corrJetP4 = corr*rawJetP4;
        if ( corrJetP4.pt() > type1JetPtThreshold_ ) {
            reco::Candidate::LorentzVector tmpP4 = jet.correctedP4(0);
            corr = getJECOffset(tmpP4, jet, jetCorrEtaMax_, offsetCorrLabel_);
            reco::Candidate::LorentzVector rawJetP4offsetCorr = corr*rawJetP4;
            corrEx    -= (corrJetP4.px() - rawJetP4offsetCorr.px());
            corrEy    -= (corrJetP4.py() - rawJetP4offsetCorr.py());
            corrSumEt += (corrJetP4.Et() - rawJetP4offsetCorr.Et());
        }
    }
    TypeICorrMap_["corrEx"]    = corrEx;
    TypeICorrMap_["corrEy"]    = corrEy;
    TypeICorrMap_["corrSumEt"] = corrSumEt;
	delete jecAK4_;
	jecAK4_=0;
	delete jecOffset_;
	jecOffset_=0;
	delete skipMuonSelection_;
	skipMuonSelection_=0;
}
//------------------------------------
math::XYZTLorentzVector
PKUTreeMaker::getNeutrinoP4(double& MetPt, double& MetPhi, TLorentzVector& lep, int lepType){
    double leppt = lep.Pt();
    double lepphi = lep.Phi();
    double lepeta = lep.Eta();
    double lepenergy = lep.Energy();
    
    double metpt = MetPt;
    double metphi = MetPhi;
    
    double  px = metpt*cos(metphi);
    double  py = metpt*sin(metphi);
    double  pz = 0;
    double  pxl= leppt*cos(lepphi);
    double  pyl= leppt*sin(lepphi);
    double  pzl= leppt*sinh(lepeta);
    double  El = lepenergy;
    double  a = pow(MW_,2) + pow(px+pxl,2) + pow(py+pyl,2) - px*px - py*py - El*El + pzl*pzl;
    double  b = 2.*pzl;
    double  A = b*b -4.*El*El;
    double  B = 2.*a*b;
    double  C = a*a-4.*(px*px+py*py)*El*El;
    ///////////////////////////pz for fnal
    double M_mu =  0;
    //if(lepType==1)M_mu=0.105658367;//mu
    //if(lepType==0)M_mu=0.00051099891;//electron
    int type=2; // use the small abs real root
    a = MW_*MW_ - M_mu*M_mu + 2.0*pxl*px + 2.0*pyl*py;
    A = 4.0*(El*El - pzl*pzl);
    B = -4.0*a*pzl;
    C = 4.0*El*El*(px*px + py*py) - a*a;
    double tmproot = B*B - 4.0*A*C;
    if (tmproot<0) {
        //std::cout << "Complex root detected, taking real part..." << std::endl;
        pz = - B/(2*A); // take real part of complex roots
    }
    else {
        double tmpsol1 = (-B + sqrt(tmproot))/(2.0*A);
        double tmpsol2 = (-B - sqrt(tmproot))/(2.0*A);
        //std::cout << " Neutrino Solutions: " << tmpsol1 << ", " << tmpsol2 << std::endl;
        if (type == 0 ) {
            // two real roots, pick the one closest to pz of muon
            if (TMath::Abs(tmpsol2-pzl) < TMath::Abs(tmpsol1-pzl)) { pz = tmpsol2; }
            else { pz = tmpsol1; }
            // if pz is > 300 pick the most central root
            if ( abs(pz) > 300. ) {
                if (TMath::Abs(tmpsol1)<TMath::Abs(tmpsol2) ) { pz = tmpsol1; }
                else { pz = tmpsol2; }
            }
        }
        if (type == 1 ) {
            // two real roots, pick the one closest to pz of muon
            if (TMath::Abs(tmpsol2-pzl) < TMath::Abs(tmpsol1-pzl)) { pz = tmpsol2; }
            else {pz = tmpsol1; }
        }
        if (type == 2 ) {
            // pick the most central root.
            if (TMath::Abs(tmpsol1)<TMath::Abs(tmpsol2) ) { pz = tmpsol1; }
            else { pz = tmpsol2; }
        }
        /*if (type == 3 ) {
         // pick the largest value of the cosine
         TVector3 p3w, p3mu;
         p3w.SetXYZ(pxl+px, pyl+py, pzl+ tmpsol1);
         p3mu.SetXYZ(pxl, pyl, pzl );
         double sinthcm1 = 2.*(p3mu.Perp(p3w))/MW_;
         p3w.SetXYZ(pxl+px, pyl+py, pzl+ tmpsol2);
         double sinthcm2 = 2.*(p3mu.Perp(p3w))/MW_;
         double costhcm1 = sqrt(1. - sinthcm1*sinthcm1);
         double costhcm2 = sqrt(1. - sinthcm2*sinthcm2);
         if ( costhcm1 > costhcm2 ) { pz = tmpsol1; otherSol_ = tmpsol2; }
         else { pz = tmpsol2;otherSol_ = tmpsol1; }
         }*///end of type3
    }//endl of if real root
    //dont correct pt neutrino
    math::XYZTLorentzVector outP4(px,py,pz,sqrt(px*px+py*py+pz*pz));
    return outP4;
}//end neutrinoP4

//------------------------------------
bool PKUTreeMaker::hasMatchedPromptElectron(const reco::SuperClusterRef &sc, const edm::Handle<edm::View<pat::Electron> > &eleCol, const edm::Handle<reco::ConversionCollection> &convCol, const math::XYZPoint &beamspot,  float lxyMin, float probMin, unsigned int nHitsBeforeVtxMax) {
    //check if a given SuperCluster matches to at least one GsfElectron having zero expected inner hits
    //and not matching any conversion in the collection passing the quality cuts
    if (sc.isNull()) return false;
    for (edm::View<pat::Electron>::const_iterator it = eleCol->begin(); it!=eleCol->end(); ++it) {
        //match electron to supercluster
        if (it->superCluster()!=sc) continue;
        //check expected inner hits
        if (it->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS) > 0) continue;
        //check if electron is matching to a conversion
        if (ConversionTools::hasMatchedConversion(*it,convCol,beamspot)) continue;
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////
int PKUTreeMaker::matchToTrueLep(double lept_eta,double lept_phi,
                                      const edm::Handle<edm::View<reco::GenParticle>>
                                      &genParticles, double &dR, int &ispromptLep)
{
    dR = 999;
    const reco::Candidate *closestLep = 0;

    int im=0;
    for(size_t i=0; i<genParticles->size();i++){
        const reco::Candidate *particle = &(*genParticles)[i];
	if (particle->pt() < 10) continue;

	if( (abs(particle->pdgId()) != 11 && abs(particle->pdgId()) != 13) || particle->status() != 1 )

        continue;
        double dRtmp = deltaR(lept_eta,lept_phi,particle->eta(),particle->phi());  
        if( dRtmp < dR ){
            dR = dRtmp;
            im=i;    
            closestLep = particle;
         }
  }

    if( !(closestLep != 0 && dR < 0.3) ) {
        return UNMATCHED;
    }
     ispromptLep=((*genParticles)[im].isPromptFinalState() || (*genParticles)[im].isDirectPromptTauDecayProductFinalState());
	if(ispromptLep && dR < 0.3)ispromptLep=1;
	else ispromptLep = 0;
return 1;
}
/////////////////////////////////////////////////////////////
//------------------------------------
int PKUTreeMaker::matchToTruth(const reco::Photon &pho,
                                      const edm::Handle<edm::View<reco::GenParticle>>
                                      &genParticles, bool &ISRPho, double &dR, int &isprompt)
{
    //
    // Explicit loop and geometric matching method
    //
    // Find the closest status 1 gen photon to the reco photon
    dR = 999;
    const reco::Candidate *closestPhoton = 0;
  //std::cout<<"genParticles->size() = "<<genParticles->size()<<std::endl;
    int im=0;
    for(size_t i=0; i<genParticles->size();i++){
        const reco::Candidate *particle = &(*genParticles)[i];
	if (particle->pt() < 10) continue;   /////////////////////////////////////////////////////////////
        if(particle->status() != 1 )
//	if( (abs(particle->pdgId()) != 11 && abs(particle->pdgId()) != 22) || particle->status() != 1 )   /////////////////////////////////////////////////////////////
//        if( abs(particle->pdgId()) != 22 || particle->status() != 1 )
        continue;
        double dRtmp = deltaR(pho.eta(),pho.phi(),particle->eta(),particle->phi());  
        if( dRtmp < dR ){
            dR = dRtmp;
            im=i;    
            closestPhoton = particle;
         }
  }
    // See if the closest photon (if it exists) is close enough.
    // If not, no match found.
    if( !(closestPhoton != 0 && dR < 0.3) ) {
        return UNMATCHED;
       // ISRPho = false;
    }
//     isprompt=(*genParticles)[im].isPromptFinalState();

     isprompt=((*genParticles)[im].isPromptFinalState() || (*genParticles)[im].isDirectPromptTauDecayProductFinalState());
        const reco::Candidate *particle = &(*genParticles)[im];
        if(abs(particle->pdgId()) == 22 && isprompt && dR >0.6)isprompt = 7;
        if(abs(particle->pdgId()) == 22 && isprompt && dR < 0.6)isprompt = 6;
        if(abs(particle->pdgId()) == 22 && isprompt && dR < 0.5)isprompt = 5;
        if(abs(particle->pdgId()) == 22 && isprompt && dR < 0.4)isprompt = 4;

        if(abs(particle->pdgId()) == 22 && isprompt && dR < 0.3)isprompt = 2;
        if(abs(particle->pdgId()) == 11 && isprompt && dR < 0.3)isprompt = 3;

    // Find ID of the parent of the found generator level photon match
    int ancestorPID = -999;
    int ancestorStatus = -999;
    findFirstNonPhotonMother(closestPhoton, ancestorPID, ancestorStatus);
    // Allowed parens: quarks pdgId 1-5, or a gluon 21
    std::vector<int> allowedParents { -1, 1, -2, 2, -3, 3, -4, 4, -5, 5, 21,-11,11,-13,13,-15,15,23,-24,24 };
    if( !(std::find(allowedParents.begin(),
                    allowedParents.end(), ancestorPID)
          != allowedParents.end()) ){
        // So it is not from g, u, d, s, c, b. Check if it is from pi0 or not.
        if( abs(ancestorPID) == 111 )
        return MATCHED_FROM_PI0;
       // ISRPho =true;
        else
//      std::cout<<"Mother = "<<abs(ancestorPID)<<" "<<closestPhoton->mother(0)->pdgId()<<" "<<closestPhoton->mother(0)->status()<<std::endl;
//      std::cout<<"Run="<<run<<" Event="<<nevent<<" lumi="<<ls<<std::endl;
        return MATCHED_FROM_OTHER_SOURCES;
      //  ISRPho =true;
    }
    return MATCHED_FROM_GUDSCB;  //Z, W ?
     //   ISRPho =true;
}
//------------------------------------

void PKUTreeMaker::findFirstNonPhotonMother(const reco::Candidate *particle,
                                                   int &ancestorPID, int &ancestorStatus){
    if( particle == 0 ){
        printf("SimplePhotonNtupler: ERROR! null candidate pointer, this should never happen\n");
        return;
    }
    // Is this the first non-photon parent? If yes, return, otherwise
    // go deeper into recursion
    if( abs(particle->pdgId()) == 22 ){
        findFirstNonPhotonMother(particle->mother(0), ancestorPID, ancestorStatus);
    }else{
        ancestorPID = particle->pdgId();
        ancestorStatus = particle->status();
    }
    return;
}
//------------------------------------
//------------------------------------
PKUTreeMaker::~PKUTreeMaker()
{
    // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}
//------------------------------------
//------------------------------------
void
PKUTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
   setDummyValues(); //Initalize variables with dummy values
   nevent = iEvent.eventAuxiliary().event();
   run    = iEvent.eventAuxiliary().run();
   ls     = iEvent.eventAuxiliary().luminosityBlock();
/*
        edm::Handle<LHEEventProduct> lheEvtInfo;
        iEvent.getByToken(LheToken_, lheEvtInfo);

	for(int icount=0; icount<lheEvtInfo->hepeup().NUP; icount++){
		if( abs(lheEvtInfo->hepeup().IDUP[icount])==22 && lheEvtInfo->hepeup().ISTUP[icount]==1)
		{    float px = (lheEvtInfo->hepeup().PUP[icount])[0];
		     float py = (lheEvtInfo->hepeup().PUP[icount])[1];
		     float pt = sqrt(px*px+py*py);
	   	     lhephoton_pt=pt;
		}
	}
*/
/*
     for(size_t i=0; i<genParticles->size();i++)
        const reco::Candidate *particle = &(*genParticles)[i];
        if( abs(particle->pdgId())== 22 && particle->status()== 1 && (*genParticles)[i].isPromptFinalState()>0 && ipp<6 ) {
            genphoton_pt[ipp]=particle->pt();
            genphoton_eta[ipp]=particle->eta();
            genphoton_phi[ipp]=particle->phi();
            ipp++;
			}
 
 */

//events weight
   if (RunOnMC_){
//  L1 prefiring
edm::Handle< double > theprefweight;
iEvent.getByToken(prefweight_token, theprefweight ) ;
L1prefiring =(*theprefweight);

edm::Handle< double > theprefweightup;
iEvent.getByToken(prefweightup_token, theprefweightup ) ;
L1prefiringup =(*theprefweightup);

edm::Handle< double > theprefweightdown;
iEvent.getByToken(prefweightdown_token, theprefweightdown ) ;
L1prefiringdown =(*theprefweightdown);

//        std::cout<<lheEvtInfo->hepeup().NUP<<std::endl; 
        edm::Handle<GenEventInfoProduct> genEvtInfo;
        iEvent.getByToken(GenToken_,genEvtInfo);
        theWeight = genEvtInfo->weight();
        if(theWeight>0) nump = nump+1;
        if(theWeight<0) numm = numm+1;

	edm::Handle<LHEEventProduct> wgtsource;
	iEvent.getByToken(LheToken_, wgtsource);
	std::cout<<"weight number "<<wgtsource->weights().size()<<std::endl;
        for ( int i=0; i<671; ++i) {
                     pweight[i]= wgtsource->weights()[i].wgt/wgtsource->originalXWGTUP();
		std::cout<<"weight= "<<pweight[i]<<std::endl;
                }



        edm::Handle<std::vector<PileupSummaryInfo>>  PupInfo;
        iEvent.getByToken(PUToken_, PupInfo);
        std::vector<PileupSummaryInfo>::const_iterator PVI;
        for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI) {
          nBX = PVI->getBunchCrossing();
          if(nBX == 0) { // "0" is the in-time crossing, negative values are the early crossings, positive are late
          npT = PVI->getTrueNumInteractions();
          npIT = PVI->getPU_NumInteractions();
          }
        } 
     } 

   Handle<TriggerResults> trigRes;
   iEvent.getByToken(hltToken_, trigRes);
   int xtemp1=0;
   for (size_t i=0; i<elPaths1.size();i++) {
      xtemp1 = (int)trigRes->accept(hltConfig.triggerIndex(elPaths1[i]));
      if(HLT_Ele1<xtemp1) HLT_Ele1=xtemp1;
   }
   int xtemp2=0;
   for (size_t i=0; i<elPaths2.size();i++) {
      xtemp2 = (int)trigRes->accept(hltConfig.triggerIndex(elPaths2[i]));
      if(HLT_Ele2<xtemp2) HLT_Ele2=xtemp2;
   }
   int mtemp1=0;
   for (size_t i=0; i<muPaths1.size();i++) {
//std::cout<<muPaths1.size()<<" "<<muPaths1[i]<<std::endl;
//std::cout<<(hltConfig.triggerIndex(muPaths1[i]))<<std::endl;
      mtemp1 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths1[i]));
      if(HLT_Mu1<mtemp1) HLT_Mu1=mtemp1;
   }
   int mtemp2=0;
   for (size_t i=0; i<muPaths2.size();i++) {
      mtemp2 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths2[i]));
      if(HLT_Mu2<mtemp2) HLT_Mu2=mtemp2;
   }
   int mtemp3=0;
   for (size_t i=0; i<muPaths3.size();i++) {
      mtemp3 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths3[i]));
      if(HLT_Mu3<mtemp3) HLT_Mu3=mtemp3;
   }

   edm::Handle<edm::View<reco::Candidate> > leptonicVs;
   iEvent.getByToken(leptonicVSrc_, leptonicVs);

   if (leptonicVs->empty()) { outTree_->Fill();  return;  }//outTree_->Fill();
 
   iEvent.getByToken(rhoToken_      , rho_     );
   double fastJetRho = *(rho_.product());
   useless = fastJetRho;
  
   edm::Handle<edm::View<pat::Jet> > ak4jets;
   iEvent.getByToken(ak4jetsSrc_, ak4jets);

   edm::Handle<edm::View<pat::Photon> > photons;
   iEvent.getByToken(photonSrc_, photons);

   if (photons->empty()) {   outTree_->Fill();return;  }//outTree_->Fill();

   edm::Handle<edm::View<reco::GenParticle> > genParticles;//define genParticle
   iEvent.getByToken(genSrc_, genParticles);
//   iEvent.getByLabel(InputTag("packedGenParticles"), genParticles);

   if (RunOnMC_){
    int ipp=0, imm=0, iee=0;
    for(size_t i=0; i<genParticles->size();i++){// std::cout<<"i = "<<i<<std::endl;
        const reco::Candidate *particle = &(*genParticles)[i];
        if( abs(particle->pdgId())== 22 && particle->status()== 1 && (*genParticles)[i].isPromptFinalState()>0 && ipp<6 ) {
            genphoton_pt[ipp]=particle->pt();
            genphoton_eta[ipp]=particle->eta();
            genphoton_phi[ipp]=particle->phi();
            ipp++;	//	std::cout<<"ipp = "<<ipp<<std::endl;
        }//std::cout<<"ipp = "<<ipp<<std::endl;
        if( abs(particle->pdgId())== 13 && particle->status()== 1 && (*genParticles)[i].isPromptFinalState()>0 && imm<6 ) {
            genmuon_pt[imm]=particle->pt();
            genmuon_eta[imm]=particle->eta();
            genmuon_phi[imm]=particle->phi();
            imm++;
        }
        if( abs(particle->pdgId())== 11 && particle->status()== 1 && (*genParticles)[i].isPromptFinalState()>0 && iee<6 ) {
            genelectron_pt[iee]=particle->pt();
            genelectron_eta[iee]=particle->eta();
            genelectron_phi[iee]=particle->phi();
            iee++;
        }
     }
   }

   edm::Handle<edm::View<pat::Muon>> loosemus;
   iEvent.getByToken(loosemuonToken_,loosemus); 
   edm::Handle<edm::View<pat::Electron>> looseeles;
   iEvent.getByToken(looseelectronToken_,looseeles); 
   edm::Handle<edm::View<reco::Candidate> > metHandle; 
   iEvent.getByToken(metSrc_, metHandle); 

//filter
   iEvent.getByToken(noiseFilterToken_, noiseFilterBits_);
   const edm::TriggerNames &names = iEvent.triggerNames(*noiseFilterBits_);
   for (unsigned int i = 0, n = noiseFilterBits_->size(); i < n; ++i) {
   if (names.triggerName(i) == HBHENoiseFilter_Selector_)
            passFilter_HBHE_ = noiseFilterBits_->accept(i);  
   if (names.triggerName(i) == HBHENoiseIsoFilter_Selector_)
            passFilter_HBHEIso_ = noiseFilterBits_->accept(i);  
   if (names.triggerName(i) ==ECALDeadCellNoiseFilter_Selector_)
	      passFilter_globalTightHalo_ = noiseFilterBits_->accept(i);  
   if (names.triggerName(i) == ECALDeadCellNoiseFilter_Selector_)
            passFilter_ECALDeadCell_ = noiseFilterBits_->accept(i);  
   if (names.triggerName(i) == GoodVtxNoiseFilter_Selector_)
            passFilter_GoodVtx_ = noiseFilterBits_->accept(i);  
   if (names.triggerName(i) == EEBadScNoiseFilter_Selector_)
            passFilter_EEBadSc_ = noiseFilterBits_->accept(i);  
   }  
     edm::Handle<bool> badMuonResultHandle;
     edm::Handle<bool> badChargedHadronResultHandle;
     iEvent.getByToken(badMuon_Selector_, badMuonResultHandle);
     iEvent.getByToken(badChargedHadron_Selector_, badChargedHadronResultHandle);
     passFilter_badMuon_ = *badMuonResultHandle;
     passFilter_badChargedHadron_ = *badChargedHadronResultHandle;

  
   const reco::Candidate& leptonicV = leptonicVs->at(0);
   const reco::Candidate& metCand = metHandle->at(0);
   const reco::Candidate& lepton = (*leptonicV.daughter(0));
       
   edm::Handle<reco::VertexCollection> vertices;
   iEvent.getByToken(VertexToken_, vertices);
   if (vertices->empty()) {  outTree_->Fill();return;} // skip the event if no PV foundoutTree_->Fill(); 
   nVtx = vertices->size();
   reco::VertexCollection::const_iterator firstGoodVertex = vertices->end();
   for (reco::VertexCollection::const_iterator vtx = vertices->begin(); vtx != vertices->end(); ++vtx) {
     // Replace isFake() for miniAOD because it requires tracks and miniAOD vertices don't have tracks:
    // Vertex.h: bool isFake() const {return (chi2_==0 && ndof_==0 && tracks_.empty());}
       if (  // !vtx->isFake() &&
           !(vtx->chi2()==0 && vtx->ndof()==0) 
           &&  vtx->ndof()>=4. && vtx->position().Rho()<=2.0
           && fabs(vtx->position().Z())<=24.0) {
           firstGoodVertex = vtx;
           break;
          }           
      }
   if ( firstGoodVertex==vertices->end() ) {  outTree_->Fill();return;} // skip event if there are no good PVsoutTree_->Fill(); 


// ************************* MET ********************** //
    edm::Handle<pat::METCollection>  METs_;
    bool defaultMET = iEvent.getByToken(metInputToken_ , METs_ );
    if(RunOnMC_){
     const pat::MET &xmet = METs_->front();
     genMET=xmet.genMET()->pt();
    }
    if(defaultMET){
        addTypeICorr(iEvent);
        for (const pat::MET &met : *METs_) {
//         const float  rawPt    = met.shiftedPt(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
//         const float  rawPhi   = met.shiftedPhi(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
//         const float  rawSumEt = met.shiftedSumEt(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
              const float rawPt = met.uncorPt();
              const float rawPhi = met.uncorPhi();
              const float rawSumEt = met.uncorSumEt();
            TVector2 rawMET_;
            rawMET_.SetMagPhi (rawPt, rawPhi );
            Double_t rawPx = rawMET_.Px();
            Double_t rawPy = rawMET_.Py();
            Double_t rawEt = std::hypot(rawPx,rawPy);
            METraw_et = rawEt;
            METraw_phi = rawPhi;
            METraw_sumEt = rawSumEt;
            double pxcorr = rawPx+TypeICorrMap_["corrEx"];
            double pycorr = rawPy+TypeICorrMap_["corrEy"];
            double et     = std::hypot(pxcorr,pycorr);
            double sumEtcorr = rawSumEt+TypeICorrMap_["corrSumEt"];
            TLorentzVector corrmet; corrmet.SetPxPyPzE(pxcorr,pycorr,0.,et);
            useless = sumEtcorr;
            useless = rawEt;
            MET_et = et;
            MET_phi = corrmet.Phi();
            MET_sumEt = sumEtcorr;
            MET_corrPx = TypeICorrMap_["corrEx"];
            MET_corrPy = TypeICorrMap_["corrEy"];
        }
    }
//------------------------------------
       /// For the time being, set these to 1
       triggerWeight=1.0;
       pileupWeight=1.0;
       double targetEvents = targetLumiInvPb_*crossSectionPb_;
       lumiWeight = targetEvents/originalNEvents_;
       lep          = abs(leptonicV.daughter(0)->pdgId());    //std::max(abs(leptonicV.daughter(0)->pdgId()), abs(leptonicV.daughter(1)->pdgId()));
       ptVlep       = leptonicV.pt();
       yVlep        = leptonicV.eta();
       phiVlep      = leptonicV.phi();
       massVlep     = leptonicV.mass();
       mtVlep       = leptonicV.mt();
       ptlep1       = leptonicV.daughter(1)->pt();
       etalep1      = leptonicV.daughter(1)->eta();
       philep1      = leptonicV.daughter(1)->phi();
       energylep1     = leptonicV.daughter(1)->energy();
       if(leptonicV.daughter(0)->isElectron()||leptonicV.daughter(0)->isMuon() ) {
       ptlep1       = leptonicV.daughter(0)->pt();
       etalep1      = leptonicV.daughter(0)->eta();
       philep1      = leptonicV.daughter(0)->phi(); 
       energylep1     = leptonicV.daughter(0)->energy(); }
       met          = metCand.pt();
       metPhi       = metCand.phi();
       mtVlepnew=sqrt(2*ptlep1*met*(1.0-cos(philep1-metPhi)));
       nlooseeles = looseeles->size(); 
       nloosemus = loosemus->size(); 

       TLorentzVector  glepton;
       glepton.SetPtEtaPhiE(ptlep1, etalep1, philep1, energylep1);
       math::XYZTLorentzVector neutrinoP4 = getNeutrinoP4(MET_et, MET_phi, glepton, 1);
       reco::CandidateBaseRef METBaseRef = metHandle->refAt(0);  //?????
       reco::ShallowCloneCandidate neutrino(METBaseRef, 0 , neutrinoP4);
       reco::CompositeCandidate WLeptonic;
       WLeptonic.addDaughter(lepton);
       WLeptonic.addDaughter(neutrino); 
       AddFourMomenta addP4;
       addP4.set(WLeptonic);
       ptVlepJEC       = WLeptonic.pt();
       yVlepJEC        = WLeptonic.eta();
       phiVlepJEC      = WLeptonic.phi();
       massVlepJEC     = WLeptonic.mass();
       mtVlepJEC       = WLeptonic.mt();
       mtVlepJECnew=sqrt(2*ptlep1*MET_et*(1.0-cos(philep1-MET_phi)));

                if(RunOnMC_ && ptlep1>10){
	//const auto lept = lepton;
	lepton_istrue=matchToTrueLep(etalep1,philep1,genParticles,dR1_,ispromptLep_);
		}


// ************************* Photon Jets Information****************** //
// *************************************************************//
         double rhoVal_;
         rhoVal_=-99.;
         rhoVal_ = *rho_;
         edm::Handle<edm::ValueMap<float> > full5x5SigmaIEtaIEtaMap;
         iEvent.getByToken(full5x5SigmaIEtaIEtaMapToken_, full5x5SigmaIEtaIEtaMap);
         edm::Handle<edm::ValueMap<float> > phoChargedIsolationMap;
         iEvent.getByToken(phoChargedIsolationToken_, phoChargedIsolationMap);
         edm::Handle<edm::ValueMap<float> > phoNeutralHadronIsolationMap;
         iEvent.getByToken(phoNeutralHadronIsolationToken_, phoNeutralHadronIsolationMap);
         edm::Handle<edm::ValueMap<float> > phoPhotonIsolationMap;
         iEvent.getByToken(phoPhotonIsolationToken_, phoPhotonIsolationMap);

         photonet=-10.; photonet_f=-10.;  iphoton=-1; iphoton_f=-1;
		int cachecount1=0; //added by Qianming Huang !!!
		int cachecount2=0; //added by Qianming Huang !!!
          for (size_t ip=0; ip<photons->size();ip++)
         {
            const auto pho = photons->ptrAt(ip);

            double phosc_eta=pho->superCluster()->eta();
            double phosc_phi=pho->superCluster()->phi();

            double pho_ieie=(*full5x5SigmaIEtaIEtaMap)[pho];
            double chIso1 =  (*phoChargedIsolationMap)[pho];
            double nhIso1 =  (*phoNeutralHadronIsolationMap)[pho];
            double phIso1 = (*phoPhotonIsolationMap)[pho];
/*
            double chiso=std::max(0.0, chIso1 - rhoVal_*EAch(fabs((*photons)[ip].eta()))); //effAreaChHadrons_.getEffectiveArea(fabs(phosc_eta)));
//            double chiso=std::max((*photons)[ip].chargedHadronIso()-rhoVal_*EAch(fabs((*photons)[ip].eta())),0.0);
            double nhiso=std::max(0.0, nhIso1 - rhoVal_*EAnh(fabs((*photons)[ip].eta()))); //effAreaNeuHadrons_.getEffectiveArea(fabs(phosc_eta)));
//            double nhiso=std::max((*photons)[ip].neutralHadronIso()-rhoVal_*EAnh(fabs((*photons)[ip].eta())),0.0);
            double phoiso=std::max(0.0, phIso1 - rhoVal_*EApho(fabs((*photons)[ip].eta()))); //effAreaPhotons_.getEffectiveArea(fabs(phosc_eta)));
//            double phoiso=std::max((*photons)[ip].photonIso()-rhoVal_*EApho(fabs((*photons)[ip].eta())),0.0);
*/
            double chiso=std::max(0.0, chIso1 - rhoVal_*EAch(fabs(phosc_eta)));
            double nhiso=std::max(0.0, nhIso1 - rhoVal_*EAnh(fabs(phosc_eta)));
            double phoiso=std::max(0.0, phIso1 - rhoVal_*EApho(fabs(phosc_eta)));


            int ismedium_photon=0;
			int ismedium_photon_f=0;
             edm::Handle<edm::View<pat::Electron> > electrons;
             iEvent.getByToken(electronToken_, electrons);
             edm::Handle<reco::BeamSpot> beamSpot;
             iEvent.getByToken(beamSpotToken_,beamSpot);
             edm::Handle<std::vector<reco::Conversion> > conversions;
             iEvent.getByToken(conversionsToken_,conversions);
             passEleVeto = (!hasMatchedPromptElectron((*photons)[ip].superCluster(),electrons, conversions, beamSpot->position() ) );
             passEleVetonew=(*photons)[ip].passElectronVeto();
             passPixelSeedVeto=(*photons)[ip].hasPixelSeed();


            if(ip<6)  {
                photon_pt[ip] = (*photons)[ip].pt();
                photon_eta[ip] = (*photons)[ip].eta();
                photon_phi[ip] = (*photons)[ip].phi();
                photon_e[ip] = (*photons)[ip].energy();
                photonsc_eta[ip] = phosc_eta;
                photonsc_phi[ip] = phosc_phi;
                photon_pev[ip]=passEleVeto;
                photon_pevnew[ip]=passEleVetonew;
                photon_ppsv[ip]=passPixelSeedVeto;
                photon_iseb[ip]=(*photons)[ip].isEB();
                photon_isee[ip]=(*photons)[ip].isEE();
                photon_hoe[ip]=(*photons)[ip].hadTowOverEm();
                photon_sieie[ip]=pho_ieie;//(*photons)[ip].sigmaIetaIeta();
                photon_sieie2[ip]=(*photons)[ip].sigmaIetaIeta();
                photon_chiso[ip]=chiso;
                photon_nhiso[ip]=nhiso;
                photon_phoiso[ip]=phoiso;
                if(RunOnMC_ && photon_pt[ip]>10){
                  const auto pho = photons->ptrAt(ip);
                  photon_istrue[ip]=matchToTruth(*pho, genParticles, ISRPho, dR_, photon_isprompt[ip]);
                 }
                photon_drla[ip]=deltaR(photonsc_eta[ip],photonsc_phi[ip],etalep1,philep1);
                TLorentzVector tp4;
                tp4.SetPtEtaPhiE(photon_pt[ip],photon_eta[ip],photon_phi[ip],photon_e[ip]);
                photon_mla[ip]=(tp4+glepton).M();
                TLorentzVector fwp4;
                fwp4.SetPtEtaPhiE(WLeptonic.pt(), WLeptonic.eta(), WLeptonic.phi(), WLeptonic.energy());
                photon_mva[ip]=(tp4+fwp4).M();
               }

            if(fabs(phosc_eta)<1.4442 && (*photons)[ip].hadTowOverEm()<0.0396 && photon_sieie[ip]<0.01022 && chiso<0.441 && nhiso<(2.725 + (0.0148*(*photons)[ip].pt()+0.000017*(*photons)[ip].pt()*(*photons)[ip].pt())) && phoiso<(2.571+0.0047*(*photons)[ip].pt())) {ismedium_photon=1;} 
            if(fabs(phosc_eta)>1.566 && fabs(phosc_eta)<2.5 && (*photons)[ip].hadTowOverEm()<0.0219 && photon_sieie[ip]<0.03001 && chiso<0.442 && nhiso<(1.715 + (0.0163*(*photons)[ip].pt()+0.000014*(*photons)[ip].pt()*(*photons)[ip].pt())) && phoiso<(3.863+0.0034*(*photons)[ip].pt())) {ismedium_photon=1;}

             if(ismedium_photon==1 && deltaR(phosc_eta,phosc_phi,etalep1,philep1) > 0.5) {if(cachecount1==0){photonet=(*photons)[ip].pt(); iphoton=ip;}cachecount1++;
                 if((*photons)[ip].pt()>photonet) {photonet=(*photons)[ip].pt(); iphoton=ip;}
         										}
//////////////////////////////////for fake photon study, store photon without sieie cut
bool barrel_inv_hoe = (*photons)[ip].hadTowOverEm()>0.0597  && chiso<0.441  && nhiso<(2.725+ (0.0148*(*photons)[ip].pt()+0.000017*(*photons)[ip].pt()*(*photons)[ip].pt())) && phoiso<(2.571+0.0047*(*photons)[ip].pt()) && photon_sieie[ip]<0.01022;
bool barrel_inv_chiso = chiso>1.295 && (*photons)[ip].hadTowOverEm()<0.0396  && nhiso<(2.725+ (0.0148*(*photons)[ip].pt()+0.000017*(*photons)[ip].pt()*(*photons)[ip].pt())) && phoiso<(2.571+0.0047*(*photons)[ip].pt()) && photon_sieie[ip]<0.01022; 
bool barrel_inv_nhiso = nhiso>(10.91+ (0.0148*(*photons)[ip].pt()+0.000017*(*photons)[ip].pt()*(*photons)[ip].pt())) && (*photons)[ip].hadTowOverEm()<0.0396  && chiso<0.441 && phoiso<(2.571+0.0047*(*photons)[ip].pt()) && photon_sieie[ip]<0.01022;
bool barrel_inv_phoiso = phoiso>(3.63+0.0047*(*photons)[ip].pt()) && (*photons)[ip].hadTowOverEm()<0.0396  && chiso<0.441  && nhiso<(2.725+ (0.0148*(*photons)[ip].pt()+0.000017*(*photons)[ip].pt()*(*photons)[ip].pt())) && photon_sieie[ip]<0.01022;
bool barrel_inv_sieie = photon_sieie[ip]>0.01031  && (*photons)[ip].hadTowOverEm()<0.0396  && chiso<0.441 && nhiso<(2.725+ (0.0148*(*photons)[ip].pt()+0.000017*(*photons)[ip].pt()*(*photons)[ip].pt())) && phoiso<(2.571+0.0047*(*photons)[ip].pt());

bool endcap_inv_hoe = (*photons)[ip].hadTowOverEm()>0.0481  && chiso<0.442  && nhiso<(1.715+ (0.0163*(*photons)[ip].pt()+0.000014*(*photons)[ip].pt()*(*photons)[ip].pt())) && phoiso<(3.863+0.0034*(*photons)[ip].pt()) && photon_sieie[ip]<0.03001;
bool endcap_inv_chiso = chiso>1.011 && (*photons)[ip].hadTowOverEm()<0.0219  && nhiso<(1.715+ (0.0163*(*photons)[ip].pt()+0.000014*(*photons)[ip].pt()*(*photons)[ip].pt())) && phoiso<(3.863+0.0034*(*photons)[ip].pt()) && photon_sieie[ip]<0.03001;
bool endcap_inv_nhiso = nhiso>(5.931+ (0.0163*(*photons)[ip].pt()+0.000014*(*photons)[ip].pt()*(*photons)[ip].pt())) && (*photons)[ip].hadTowOverEm()<0.0219  && chiso<0.442 && phoiso<(3.863+0.0034*(*photons)[ip].pt()) && photon_sieie[ip]<0.03001;
bool endcap_inv_phoiso = phoiso>(6.641+0.0034*(*photons)[ip].pt()) && (*photons)[ip].hadTowOverEm()<0.0219  && chiso<0.442  && nhiso<(1.715+ (0.0163*(*photons)[ip].pt()+0.000014*(*photons)[ip].pt()*(*photons)[ip].pt())) && photon_sieie[ip]<0.03001;
bool endcap_inv_sieie = photon_sieie[ip]>0.03013  && (*photons)[ip].hadTowOverEm()<0.0219  && chiso<0.442 && nhiso<(1.715+ (0.0163*(*photons)[ip].pt()+0.000014*(*photons)[ip].pt()*(*photons)[ip].pt())) && phoiso<(3.863+0.0034*(*photons)[ip].pt());

            if(fabs(phosc_eta)<1.4442 && (barrel_inv_hoe || barrel_inv_chiso || barrel_inv_nhiso || barrel_inv_phoiso || barrel_inv_sieie)){ismedium_photon_f=1;}
            if(fabs(phosc_eta)>1.566 && fabs(phosc_eta)<2.5  && (endcap_inv_hoe || endcap_inv_chiso || endcap_inv_nhiso || endcap_inv_phoiso || endcap_inv_sieie)){ismedium_photon_f=1;}


            if(ismedium_photon_f==1 && deltaR(phosc_eta,phosc_phi,etalep1,philep1) > 0.5)
                {
                if(cachecount2==0) {photonet_f=(*photons)[ip].pt(); iphoton_f=ip;}cachecount2++;
                if((*photons)[ip].pt()>photonet_f) {photonet_f=(*photons)[ip].pt(); iphoton_f=ip;}
		}		 
////////////////////////////////////////////////////////////////////////////////////////		 
	 }

             //Gen photon matching
    if(RunOnMC_ && iphoton>-1){
             const auto pho1 = photons->ptrAt(iphoton);
             isTrue_= matchToTruth(*pho1, genParticles, ISRPho, dR_, isprompt_);
    }

         if(iphoton>-1 && iphoton<6) {
               photonet=photon_pt[iphoton];//(*photons)[iphoton].pt();
               photoneta=photon_eta[iphoton];//(*photons)[iphoton].eta();
               photonphi=photon_phi[iphoton];//(*photons)[iphoton].phi();
               photone=photon_e[iphoton];//(*photons)[iphoton].energy();
		photonsceta=photonsc_eta[iphoton];
		photonscphi=photonsc_phi[iphoton];
               photonsieie=photon_sieie[iphoton];//(*photons)[iphoton].sigmaIetaIeta();
               photonphoiso=photon_phoiso[iphoton];//std::max((*photons)[iphoton].photonIso()-rhoVal_*EApho(fabs((*photons)[iphoton].eta())),0.0);
               photonchiso=photon_chiso[iphoton];//std::max((*photons)[iphoton].chargedHadronIso()-rhoVal_*EAch(fabs((*photons)[iphoton].eta())),0.0);
               photonnhiso=photon_nhiso[iphoton];//std::max((*photons)[iphoton].neutralHadronIso()-rhoVal_*EAnh(fabs((*photons)[iphoton].eta())),0.0);
               drla=deltaR(photonsc_eta[iphoton],photonsc_phi[iphoton],etalep1,philep1);
               TLorentzVector photonp4;
               photonp4.SetPtEtaPhiE(photonet, photoneta, photonphi, photone);
               Mla=(photonp4+glepton).M();
               TLorentzVector wp4;
               wp4.SetPtEtaPhiE(WLeptonic.pt(), WLeptonic.eta(), WLeptonic.phi(), WLeptonic.energy());
               Mva=(photonp4+wp4).M();
		photonhaspixelseed=photon_ppsv[iphoton];
		photonpasseleveto=photon_pevnew[iphoton];
         }


         if(iphoton_f>-1 && iphoton_f<6) {
		       photonet_f=photon_pt[iphoton_f];//(*photons)[iphoton_f].pt();
		       photoneta_f=photon_eta[iphoton_f];//(*photons)[iphoton_f].eta();
		       photonphi_f=photon_phi[iphoton_f];//(*photons)[iphoton_f].phi();
		       photone_f=photon_e[iphoton_f];//(*photons)[iphoton_f].energy();
                       photonsceta_f=photonsc_eta[iphoton_f];
                       photonscphi_f=photonsc_phi[iphoton_f];
		       photonsieie_f=photon_sieie[iphoton_f];//(*photons)[iphoton_f].sigmaIetaIeta();
		       photonphoiso_f=photon_phoiso[iphoton_f];//std::max((*photons)[iphoton_f].photonIso()-rhoVal_*EApho(fabs((*photons)[iphoton_f].eta())),0.0);
		       photonchiso_f=photon_chiso[iphoton_f];//std::max((*photons)[iphoton_f].chargedHadronIso()-rhoVal_*EAch(fabs((*photons)[iphoton_f].eta())),0.0);
		       photonnhiso_f=photon_nhiso[iphoton_f];//std::max((*photons)[iphoton_f].neutralHadronIso()-rhoVal_*EAnh(fabs((*photons)[iphoton_f].eta())),0.0);
		       drla_f=deltaR(photonsc_eta[iphoton_f],photonsc_phi[iphoton_f],etalep1,philep1);
		       TLorentzVector photonp4_f;
		       photonp4_f.SetPtEtaPhiE(photonet_f, photoneta_f, photonphi_f, photone_f);
	           Mla_f=(photonp4_f+glepton).M();
	           TLorentzVector wp4_f;
	           wp4_f.SetPtEtaPhiE(WLeptonic.pt(), WLeptonic.eta(), WLeptonic.phi(), WLeptonic.energy());
	           Mva_f=(photonp4_f+wp4_f).M();
			photonhaspixelseed_f=photon_ppsv[iphoton_f];
			photonpasseleveto_f=photon_pevnew[iphoton_f];
	     }

// ************************* AK4 Jets Information****************** //
// ***********************************************************//
    Int_t jetindexphoton12[2] = {-1,-1}; 
	Int_t jetindexphoton12_f[2] = {-1,-1};

    std::vector<JetCorrectorParameters> vPar;
    for ( std::vector<std::string>::const_iterator payloadBegin = jecAK4Labels_.begin(), payloadEnd = jecAK4Labels_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
         JetCorrectorParameters pars(*ipayload);
         vPar.push_back(pars);    }
    jecAK4_ = new FactorizedJetCorrector(vPar);
    vPar.clear();

    int nujets=0 ;
    double tmpjetptcut=20.0;
    std::vector<TLorentzVector*> jets;
   
//################Jet Correction##########################
        for (size_t ik=0; ik<ak4jets->size();ik++)
         {
            reco::Candidate::LorentzVector uncorrJet = (*ak4jets)[ik].correctedP4(0);
            jecAK4_->setJetEta( uncorrJet.eta() );
            jecAK4_->setJetPt ( uncorrJet.pt() );
            jecAK4_->setJetE ( uncorrJet.energy() );
            jecAK4_->setRho ( rhoVal_ );
            jecAK4_->setNPV ( vertices->size() );
            jecAK4_->setJetA ( (*ak4jets)[ik].jetArea() );
            double corr = jecAK4_->getCorrection();

            if(corr*uncorrJet.pt()>tmpjetptcut) {
            TLorentzVector *dummy = new TLorentzVector(0,0,0,0);    
            dummy->SetPtEtaPhiE(corr*uncorrJet.pt(), uncorrJet.eta(), uncorrJet.phi(), corr*uncorrJet.energy());
            jets.push_back(dummy);
            ++nujets;
            }   
            if(ik<6)  {  
                ak4jet_hf[ik]=(*ak4jets)[ik].hadronFlavour();
                ak4jet_pf[ik]=(*ak4jets)[ik].partonFlavour(); 
                ak4jet_pt[ik] =  corr*uncorrJet.pt();
                ak4jet_eta[ik] = (*ak4jets)[ik].eta();
                ak4jet_phi[ik] = (*ak4jets)[ik].phi();
                ak4jet_e[ik] =   corr*uncorrJet.energy();
                ak4jet_csv[ik] = (*ak4jets)[ik].bDiscriminator("pfCombinedSecondaryVertexV2BJetTags");
                ak4jet_icsv[ik] = (*ak4jets)[ik].bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");   }
          }
    
       sort (jets.begin (), jets.end (), mysortPt);

           for (size_t i=0;i<jets.size();i++) {
             if(iphoton>-1) {    
              double drtmp1=deltaR(jets.at(i)->Eta(), jets.at(i)->Phi(), photonsceta,photonscphi);
               if(drtmp1>0.5 && jetindexphoton12[0]==-1 && jetindexphoton12[1]==-1) {
                     jetindexphoton12[0] = i;
                     continue;  // the first num
               }
              if(drtmp1>0.5 && jetindexphoton12[0]!=-1 && jetindexphoton12[1]==-1) {
                     jetindexphoton12[1] = i;
                     continue; // the second num
               }
              }
           }

	   for (size_t i=0;i<jets.size();i++) {
	     if(iphoton_f>-1) {    
	      double drtmp1_f=deltaR(jets.at(i)->Eta(), jets.at(i)->Phi(), photonsceta_f,photonscphi_f);
	       if(drtmp1_f>0.5 && jetindexphoton12_f[0]==-1 && jetindexphoton12_f[1]==-1) {
		    jetindexphoton12_f[0] = i;
		    continue;  // the first num
	       }
              if(drtmp1_f>0.5 && jetindexphoton12_f[0]!=-1 && jetindexphoton12_f[1]==-1) {
	            jetindexphoton12_f[1] = i;
	            continue; // the second num
	       }
	      }
	    }

         if(jetindexphoton12[0]>-1 && jetindexphoton12[1]>-1) {

	jet1hf=(*ak4jets)[jetindexphoton12[0]].hadronFlavour();
	jet1pf=(*ak4jets)[jetindexphoton12[0]].partonFlavour(); 
	jet2hf=(*ak4jets)[jetindexphoton12[1]].hadronFlavour();
	jet2pf=(*ak4jets)[jetindexphoton12[1]].partonFlavour(); 
            jet1pt=jets[jetindexphoton12[0]]->Pt();
            jet1eta=jets[jetindexphoton12[0]]->Eta();
            jet1phi=jets[jetindexphoton12[0]]->Phi();
            jet1e=jets[jetindexphoton12[0]]->E();
            jet2pt=jets[jetindexphoton12[1]]->Pt();
            jet2eta=jets[jetindexphoton12[1]]->Eta();
            jet2phi=jets[jetindexphoton12[1]]->Phi();
            jet2e=jets[jetindexphoton12[1]]->E();
            jet1csv =(*ak4jets)[jetindexphoton12[0]].bDiscriminator("pfCombinedSecondaryVertexV2BJetTags");
            jet2csv =(*ak4jets)[jetindexphoton12[1]].bDiscriminator("pfCombinedSecondaryVertexV2BJetTags");
            jet1icsv =(*ak4jets)[jetindexphoton12[0]].bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
            jet2icsv =(*ak4jets)[jetindexphoton12[1]].bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
            drj1a=deltaR(jet1eta,jet1phi,photonsceta,photonscphi);
            drj2a=deltaR(jet2eta,jet2phi,photonsceta,photonscphi);
            drj1l=deltaR(jet1eta,jet1phi,etalep1,philep1);
            drj2l=deltaR(jet2eta,jet2phi,etalep1,philep1);
            TLorentzVector j1p4;
            j1p4.SetPtEtaPhiE(jet1pt, jet1eta, jet1phi, jet1e);
            TLorentzVector j2p4;
            j2p4.SetPtEtaPhiE(jet2pt, jet2eta, jet2phi, jet2e);
            TLorentzVector photonp42;
            photonp42.SetPtEtaPhiE(photonet, photoneta, photonphi, photone);
            TLorentzVector vp4;
//            vp4.SetPtEtaPhiE(leptonicV.pt(), leptonicV.eta(), leptonicV.phi(), leptonicV.energy());
            vp4.SetPtEtaPhiE(WLeptonic.pt(), WLeptonic.eta(), WLeptonic.phi(), WLeptonic.energy());
            j1metPhi=fabs(jet1phi-MET_phi);
            if(j1metPhi>Pi) {j1metPhi=2.0*Pi-j1metPhi;}
            j2metPhi=fabs(jet2phi-MET_phi);
            if(j2metPhi>Pi) {j2metPhi=2.0*Pi-j2metPhi;}
            Mjj=(j1p4 + j2p4).M();
            deltaeta = fabs(jet1eta - jet2eta);
            zepp = fabs((vp4+photonp42).Rapidity() - (j1p4.Rapidity() + j2p4.Rapidity())/ 2.0); 
		Dphiwajj=fabs((vp4+photonp42).Phi()-(j1p4+j2p4).Phi());
		if(Dphiwajj>Pi){Dphiwajj=2.0*Pi-Dphiwajj;}
         }



         if(jetindexphoton12_f[0]>-1 && jetindexphoton12_f[1]>-1) {

	jet1hf_f=(*ak4jets)[jetindexphoton12_f[0]].hadronFlavour();
	jet1pf_f=(*ak4jets)[jetindexphoton12_f[0]].partonFlavour(); 
	jet2hf_f=(*ak4jets)[jetindexphoton12_f[1]].hadronFlavour();
	jet2pf_f=(*ak4jets)[jetindexphoton12_f[1]].partonFlavour(); 
	    jet1pt_f=jets[jetindexphoton12_f[0]]->Pt();
            jet1eta_f=jets[jetindexphoton12_f[0]]->Eta();
            jet1phi_f=jets[jetindexphoton12_f[0]]->Phi();
            jet1e_f=jets[jetindexphoton12_f[0]]->E();
            jet2pt_f=jets[jetindexphoton12_f[1]]->Pt();
            jet2eta_f=jets[jetindexphoton12_f[1]]->Eta();
            jet2phi_f=jets[jetindexphoton12_f[1]]->Phi();
            jet2e_f=jets[jetindexphoton12_f[1]]->E();
            jet1csv_f =(*ak4jets)[jetindexphoton12_f[0]].bDiscriminator("pfCombinedSecondaryVertexV2BJetTags");
            jet2csv_f =(*ak4jets)[jetindexphoton12_f[1]].bDiscriminator("pfCombinedSecondaryVertexV2BJetTags");
            jet1icsv_f =(*ak4jets)[jetindexphoton12_f[0]].bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
            jet2icsv_f =(*ak4jets)[jetindexphoton12_f[1]].bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
	    drj1a_f=deltaR(jet1eta_f,jet1phi_f,photonsceta_f,photonscphi_f);
            drj2a_f=deltaR(jet2eta_f,jet2phi_f,photonsceta_f,photonscphi_f);
            drj1l_f=deltaR(jet1eta_f,jet1phi_f,etalep1,philep1);
            drj2l_f=deltaR(jet2eta_f,jet2phi_f,etalep1,philep1);
            TLorentzVector j1p4_f;
            j1p4_f.SetPtEtaPhiE(jet1pt_f, jet1eta_f, jet1phi_f, jet1e_f);
            TLorentzVector j2p4_f;
            j2p4_f.SetPtEtaPhiE(jet2pt_f, jet2eta_f, jet2phi_f, jet2e_f);
            TLorentzVector photonp42_f;
            photonp42_f.SetPtEtaPhiE(photonet_f, photoneta_f, photonphi_f, photone_f);
            TLorentzVector vp4_f;
//            vp4.SetPtEtaPhiE(leptonicV.pt(), leptonicV.eta(), leptonicV.phi(), leptonicV.energy());
            vp4_f.SetPtEtaPhiE(WLeptonic.pt(), WLeptonic.eta(), WLeptonic.phi(), WLeptonic.energy());
            j1metPhi_f=fabs(jet1phi_f-MET_phi);
            if(j1metPhi_f>Pi) {j1metPhi_f=2.0*Pi-j1metPhi_f;}
            j2metPhi_f=fabs(jet2phi_f-MET_phi);
            if(j2metPhi_f>Pi) {j2metPhi_f=2.0*Pi-j2metPhi_f;}
            Mjj_f=(j1p4_f + j2p4_f).M();
	    deltaeta_f = fabs(jet1eta_f - jet2eta_f);
            zepp_f = fabs((vp4_f+photonp42_f).Rapidity() - (j1p4_f.Rapidity() + j2p4_f.Rapidity())/ 2.0);
		Dphiwajj_f=fabs((vp4_f+photonp42_f).Phi()-(j1p4_f+j2p4_f).Phi());
		if(Dphiwajj_f>Pi){Dphiwajj_f=2.0*Pi-Dphiwajj_f;}
         }


       outTree_->Fill();
	   delete jecAK4_;
	   jecAK4_=0;
   }
   
//-------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------//


void PKUTreeMaker::setDummyValues() {
     npT=-1.;
     npIT=-1.;
     nBX=-1;
     nevent=-1e1;
     run=-1e1;
     ls=-1e1;
     nVtx           = -1e1;
     triggerWeight  = -1e1;
     pileupWeight   = -1e1;
     lumiWeight     = -1e1;
     theWeight = -99;
L1prefiring = -99;
L1prefiringup = -99;
L1prefiringdown = -99;
     lep            = -1e1;
     nlooseeles=-1e1;
     nloosemus=-1e1;
     ptVlep         = -1e1;
     yVlep          = -1e1;
     phiVlep        = -1e1;
     massVlep       = -1e1;
     mtVlep         = -1e1;
     mtVlepnew         = -1e1;
     ptVlepJEC         = -1e1;
     yVlepJEC          = -1e1;
     phiVlepJEC        = -1e1;
     massVlepJEC       = -1e1;
     mtVlepJEC         = -1e1;
     mtVlepJECnew      = -1e1;
     Mla=-1e1;		Mla_f=-1e1;	
     Mva=-1e1;		Mva_f=-1e1;
     ptlep1         = -1e1;
     etalep1        = -1e1;
     philep1        = -1e1;
     energylep1     = -1e1;
     met            = -1e1;
     metPhi         = -1e1;
     j1metPhi         = -1e1; j1metPhi_f         = -1e1;
     j2metPhi         = -1e1; j2metPhi_f         = -1e1;
	Dphiwajj      = -1e1; Dphiwajj_f         = -1e1;
     rawPt = -99;
     METraw_et = -99;
     METraw_phi = -99;
     METraw_sumEt = -99;
     genMET=-99;
     MET_et = -99;
     MET_phi = -99;
     MET_sumEt = -99;
     MET_corrPx = -99;
     MET_corrPy = -99;

	for(int j=0; j<671; j++){
		pweight[j]=0.0;
	}

     for(int i=0; i<6; i++) {
ak4jet_hf[i]=-1e1;
ak4jet_pf[i]=-1e1;
     genphoton_pt[i] = -1e1;
     genphoton_eta[i] = -1e1;
     genphoton_phi[i] = -1e1;
     genmuon_pt[i] = -1e1;
     genmuon_eta[i] = -1e1;
     genmuon_phi[i] = -1e1;
     genelectron_pt[i] = -1e1;
     genelectron_eta[i] = -1e1;
     genelectron_phi[i] = -1e1;
     photon_pt[i] = -1e1;
     photon_eta[i] = -1e1;
     photon_phi[i] = -1e1;
     photon_e[i] = -1e1;
     photonsc_eta[i] = -1e1;
     photonsc_phi[i] = -1e1;
     photon_pev[i] = false;
     photon_pevnew[i] = false;
     photon_ppsv[i] = false;
     photon_iseb[i] = false;
     photon_isee[i] = false;
     photon_hoe[i] = -1e1;
     photon_sieie[i] = -1e1;
     photon_sieie2[i] = -1e1;
     photon_chiso[i] = -1e1;
     photon_nhiso[i] = -1e1;
     photon_phoiso[i] = -1e1;
     photon_istrue[i] = -1;
     photon_isprompt[i] = -1;
     photon_drla[i]=1e1;
     photon_mla[i]=-1e1;
     photon_mva[i]=-1e1;
     ak4jet_pt[i]=-1e1;
     ak4jet_eta[i]=-1e1;
     ak4jet_phi[i]=-1e1;
     ak4jet_e[i]=-1e1;
     ak4jet_csv[i]=-1e1;
     ak4jet_icsv[i]=-1e1;
     }

     photonet=-1e1;	 photonet_f=-1e1;
     photoneta=-1e1;  photoneta_f=-1e1;
     photonphi=-1e1;  photonphi_f=-1e1;
     photone=-1e1;   photone_f=-1e1;
     photonsceta=-1e1;  photonsceta_f=-1e1;
     photonscphi=-1e1;  photonscphi_f=-1e1;
     photonsieie=-1e1;  photonsieie_f=-1e1;
     photonphoiso=-1e1;  photonphoiso_f=-1e1;
     photonchiso=-1e1;  photonchiso_f=-1e1;
     photonnhiso=-1e1;  photonnhiso_f=-1e1;
     iphoton=-1;	 iphoton_f=-1;
     drla=1e1; 		 drla_f=1e1;
     passEleVeto=false;
     passEleVetonew=false;
     passPixelSeedVeto=false;
	photonhaspixelseed=false;
	photonhaspixelseed_f=false;
	photonpasseleveto=false;
	photonpasseleveto_f=false;
   
 
     ISRPho = false;
     dR_ = 999;
     dR1_ = 999;
     isTrue_=-1;
     isprompt_=-1; 
     ispromptLep_=-1; 
     lepton_istrue=-1;

     jet1pt=-1e1;  jet1pt_f=-1e1;
     jet1eta=-1e1;  jet1eta_f=-1e1;
     jet1phi=-1e1;  jet1phi_f=-1e1;
     jet1e=-1e1;  jet1e_f=-1e1;
     jet1csv=-1e1;  jet1csv_f=-1e1;
     jet1icsv=-1e1;  jet1icsv_f=-1e1;
     jet2pt=-1e1;  jet2pt_f=-1e1;
     jet2eta=-1e1;  jet2eta_f=-1e1;
     jet2phi=-1e1;  jet2phi_f=-1e1;
     jet2e=-1e1;  jet2e_f=-1e1;
     jet2csv=-1e1;  jet2csv_f=-1e1;
     jet2icsv=-1e1;  jet2icsv_f=-1e1;
     drj1a=-1e1;  drj1a_f=-1e1;
     drj2a=-1e1;  drj2a_f=-1e1;
     drj1l=-1e1;  drj1l_f=-1e1;
     drj2l=-1e1;  drj2l_f=-1e1;
     Mjj=-1e1;   Mjj_f=-1e1;
     deltaeta=-1e1;   deltaeta_f=-1e1;
     zepp=-1e1;   zepp_f=-1e1;
  
     HLT_Ele1=-99;
     HLT_Ele2=-99;
     HLT_Mu1=-99;
     HLT_Mu2=-99;
     HLT_Mu3=-99;

jet1hf=-1e1;
jet1pf=-1e1;
jet2hf=-1e1;
jet2pf=-1e1;
jet1hf_f=-1e1;
jet1pf_f=-1e1;
jet2hf_f=-1e1;
jet2pf_f=-1e1;

 
     passFilter_HBHE_                  = false;
     passFilter_HBHEIso_               = false;
     passFilter_globalTightHalo_	=false;
     passFilter_ECALDeadCell_          = false;
     passFilter_GoodVtx_               = false;
     passFilter_EEBadSc_               = false;
     passFilter_badMuon_               = false;
     passFilter_badChargedHadron_      = false; 
}

// ------------ method called once each job just before starting event loop  ------------
void 
PKUTreeMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void PKUTreeMaker::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
 {

  elPaths1.clear();
  elPaths2.clear();
  muPaths1.clear();
  muPaths2.clear();
  muPaths3.clear();
  

  std::cout<<"-----begin-----"<<std::endl; 
   bool changed;
   if ( !hltConfig.init(iRun, iSetup, "HLT", changed) ) {
        edm::LogError("HltAnalysis") << "Initialization of HLTConfigProvider failed!!";
       return;
      }
   for (size_t i = 0; i < elPaths1_.size(); i++) {
         std::vector<std::string> foundPaths1 = hltConfig.matched( hltConfig.triggerNames(), elPaths1_[i] );
         while ( !foundPaths1.empty() ){
               elPaths1.push_back( foundPaths1.back() );
               foundPaths1.pop_back(); }
                                                }
   for (size_t i = 0; i < muPaths1_.size(); i++) {
         std::vector<std::string> foundPaths1 = hltConfig.matched( hltConfig.triggerNames(), muPaths1_[i] );
         while ( !foundPaths1.empty() ){
               muPaths1.push_back( foundPaths1.back() );
               foundPaths1.pop_back();
                                      }
                                                }
   std::cout<<"\n************** HLT-1 Information **************\n";
   for (size_t i=0; i < elPaths1.size(); i++) std::cout << "\n Electron paths-1:    " << i<<"  "<<elPaths1[i].c_str() <<"\t"<< std::endl;
   for (size_t i=0; i < muPaths1.size(); i++) std::cout << "\n Muon paths-1:   " << i<<"  "<<muPaths1[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < elPaths2_.size(); i++) {
         std::vector<std::string> foundPaths2 = hltConfig.matched( hltConfig.triggerNames(), elPaths2_[i] );
         while ( !foundPaths2.empty() ){
               elPaths2.push_back( foundPaths2.back() );
               foundPaths2.pop_back();
                                      }
                                                }
   for (size_t i = 0; i < muPaths2_.size(); i++) {
         std::vector<std::string> foundPaths2 = hltConfig.matched( hltConfig.triggerNames(), muPaths2_[i] );
         while ( !foundPaths2.empty() ){
               muPaths2.push_back( foundPaths2.back() );
               foundPaths2.pop_back();
                                      }
                                                }
   std::cout<<"\n************** HLT-2 Information **************\n";
   for (size_t i=0; i < elPaths2.size(); i++) std::cout << "\n Electron paths-2:    " << i<<"  "<<elPaths2[i].c_str() <<"\t"<< std::endl;
   for (size_t i=0; i < muPaths2.size(); i++) std::cout << "\n Muon paths-2:   " << i<<"  "<<muPaths2[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";
   for (size_t i = 0; i < muPaths3_.size(); i++) {
         std::vector<std::string> foundPaths3 = hltConfig.matched( hltConfig.triggerNames(), muPaths3_[i] );
         while ( !foundPaths3.empty() ){
               muPaths3.push_back( foundPaths3.back() );
               foundPaths3.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-3 Information **************\n";
   for (size_t i=0; i < muPaths3.size(); i++) std::cout << "\n Muon paths-3:   " << i<<"  "<<muPaths3[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";



}
//temp5
//temp on window
void PKUTreeMaker::endRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
PKUTreeMaker::endJob() {
  std::cout << "PKUTreeMaker endJob()..." << std::endl;
}

//define this as a plug-in
DEFINE_FWK_MODULE(PKUTreeMaker);
