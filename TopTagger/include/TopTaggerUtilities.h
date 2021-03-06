#ifndef TOPTAGGERUTILITIES_H
#define TOPTAGGERUTILITIES_H

/** \file */

#include <vector>
#include <map>

class Constituent;
class TopObject;
class TopTaggerResults;
class TF1;

#include "TLorentzVector.h"

//This file is for utility functions which do not fit in other classes 
//DO NOT DEFINE FUNCTIONS IN LINE!!!!!!

namespace ttUtility
{
    /**
     *Class to hold the gen level inputs.
     */
    class ConstGenInputs
    {
    protected:
        const std::vector<TLorentzVector>* hadGenTops_;
        const std::vector<std::vector<const TLorentzVector*>>* hadGenTopDaughters_;

        ///Default constructor
        ConstGenInputs();
        /**
         *Constructs gen inputs from gen level vectors
         *@param hadGenTops Vector of hadronicly decaying gen top TLorentzVectors
         *@param hadGenTopDaughters Vector of direct decay daughters of the top quarks
         */
        ConstGenInputs(const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
    };

    /**
     *Class to gather the information necessary to construct the AK4 jet constituents
     */
    class ConstAK4Inputs : public ConstGenInputs
    {
    private:
        const std::vector<TLorentzVector>* jetsLVec_;
        const std::vector<double>* btagFactors_;
        const std::vector<double>* qgLikelihood_;
        const std::vector<int>* qgMult_;
        const std::vector<double>* qgPtD_;
        const std::vector<double>* qgAxis1_;
        const std::vector<double>* qgAxis2_;

        std::map<std::string, const std::vector<double>*> extraInputVariables_;

    public:
        /**
         *Basic constructor with QGL
         *@param jetsLVec Jet TLorentzVectors for each jet
         *@param btagFactors B-tag discriminators for each jet
         *@param qgLikelihood Quark-gluon likelihoods for each jet
         */
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood);
        /**
         *Basic constructor
         *@param jetsLVec Jet TLorentzVectors for each jet
         *@param btagFactors B-tag discriminators for each jet
         */
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors);
        /**
         *Constructor with gen informaion 
         *@param jetsLVec Jet TLorentzVectors for each jet
         *@param btagFactors B-tag discriminators for each jet
         *@param qgLikelihood Quark-gluon likelihoods for each jet
         *@param hadGenTops Vector of hadronicly decaying gen top TLorentzVectors
         *@param hadGenTopDaughters Vector of direct decay daughters of the top quarks
         */
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        /**
         *Adds jet shape inputs from the quark-gluon likelihood calculator
         */
        void addQGLVectors(const std::vector<int>& qgMult, const std::vector<double>& qgPtD, const std::vector<double>& qgAxis1, const std::vector<double>& qgAxis2);
        /**
         *Adds a vector holding additional variables which will be inserted into the "extraVars" map of the Constituent
         *@param name The name to use to store the extra variables
         *@param vector the values for the extra variable for each jet
         */
        void addSupplamentalVector(const std::string& name, const std::vector<double>& vector);
        /**
         *Called to fill the constituents using the information collected in the class. 
         *Not intended to be called directly.
         *@param constituents vector to insert AK4 constituents into
         */
        void packageConstituents(std::vector<Constituent>& constituents);
    };

    /**
     *Class to gather the information necessary to construct the AK8 jet constituents
     */
    class ConstAK8Inputs : public ConstGenInputs
    {
    private:
        const std::vector<TLorentzVector>* jetsLVec_;
        const std::vector<double>* tau1_;
        const std::vector<double>* tau2_;
        const std::vector<double>* tau3_;
        const std::vector<double>* softDropMass_;
        const std::vector<double>* subjetsBtag_;
        const std::vector<double>* subjetsMult_;
        const std::vector<double>* subjetsPtD_;
        const std::vector<double>* subjetsAxis1_;
        const std::vector<double>* subjetsAxis2_;
        const std::vector<TLorentzVector>* subjetsLVec_;
        const std::vector<std::vector<TLorentzVector>>* vecSubjetsLVec_;
        TF1* puppisd_corrGEN_;
        TF1* puppisd_corrRECO_cen_;
        TF1* puppisd_corrRECO_for_;
        
        double getPUPPIweight(double puppipt, double puppieta ) const;

    public:
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<double>& subjetsBtag, const std::vector<double>& subjetsMult, const std::vector<double>& subjetsPtD, const std::vector<double>& subjetsAxis1, const std::vector<double>& subjetsAxis2);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<double>& subjetsBtag, const std::vector<double>& subjetsMult, const std::vector<double>& subjetsPtD, const std::vector<double>& subjetsAxis1, const std::vector<double>& subjetsAxis2, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        void packageConstituents(std::vector<Constituent>& constituents);
        std::vector<TLorentzVector> denominator(const double ptCut) const;
        void setWMassCorrHistos(const std::string& fname);
        void setWMassCorrHistos(TF1* puppisd_corrGEN, TF1* puppisd_corrRECO_cen, TF1* puppisd_corrRECO_for);

        static void prepHistosForWCorrectionFactors(const std::string& fname, TF1* puppisd_corrGEN, TF1* puppisd_corrRECO_cen, TF1* puppisd_corrRECO_for);
    };

    //template metaprogramming magic 
    ///Resurcive function to assemble constituents from arbitrary list of input classes. Don't call this function!
    template<typename T, typename... Args> void packageConstituentsRecurse(std::vector<Constituent>& constituents, T input, Args... args)
    {
        input.packageConstituents(constituents);

        packageConstituentsRecurse(constituents, args...);
    }

    ///recursion termimnation specialization. Don't call this function!
    template<typename T> void packageConstituentsRecurse(std::vector<Constituent>& constituents, T input)
    {
        input.packageConstituents(constituents);
    }

    ///Function to fill constituent list based upon arbitrary list of input objects
    template<typename... Args> std::vector<Constituent> packageConstituents(Args... args)
    {
        //vector holding constituents to be created
        std::vector<Constituent> constituents;

        //begin the recursion to fill constituents
        packageConstituentsRecurse(constituents, args...);

        return constituents;        
    }

    ///backwards compatability overload
    std::vector<Constituent> packageConstituents(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood);
    
    ///Tool to calcualte MT2 from tagger results
    double calculateMT2(const TopTaggerResults& ttr, const TLorentzVector& metLVec);

    //MVA helper functions
    /* \fn std::map<std::string, double> ttUtility::createMVAInputs(const TopObject& topCand, const double csvThresh) */
    ///Function to calculate MVA input variables from a TopObject. This function is depricated in favor of the new MVAInputCalculator based approach.
    std::map<std::string, double> createMVAInputs(const TopObject& topCand, const double csvThresh);

    //New MVA variable helper class
    /**
     *Base class for MVA input variable calculator 
     */
    class MVAInputCalculator
    {
    private:
    protected:
        float* basePtr_;
        int len_;
    public:
        /**
         *The job of mapVars is to populate the internal offests for all variables in the input variable list with their memory location in the data array.  To be called only once.
         *@param vars list of variables used for the model
         */
        virtual void mapVars(const std::vector<std::string>&) = 0;
        /**
         *The job of setPtr is to set hte starting place of memory block where the data will be written. To be called only once for the creation of the array pointed to by data.
         *@param data pointer to start of the data array which will be used as input to the MVA
         */
        virtual void setPtr(float* data) {basePtr_ = data;}
        /**
         *Calculate the requested variables and store the values directly in the input array for the MVA
         *@param topCand the top candidate to calculate the input variables for 
         */
        virtual bool calculateVars(const TopObject&, int) = 0;
        /**
         *Check if the TopObject passes basic selection for this category.
         *@param topCand the top candidate to check
         */
        virtual bool checkCand(const TopObject&) = 0;
    };

    /**
     *Class to calculate the input variables for the BDT based AK8 top selection
     */
    class BDTMonojetInputCalculator : public MVAInputCalculator
    {
    private:
        int ak8_sdmass_, ak8_tau21_, ak8_tau32_, ak8_ptDR_, ak8_rel_ptdiff_, ak8_csv1_mass_, ak8_csv1_csv_, ak8_csv1_ptD_, ak8_csv1_axis1_, ak8_csv1_mult_, ak8_csv2_mass_, ak8_csv2_ptD_, ak8_csv2_axis1_, ak8_csv2_mult_;
    public:
        BDTMonojetInputCalculator();
        void mapVars(const std::vector<std::string>&);
        bool calculateVars(const TopObject&, int);
        bool checkCand(const TopObject&);
    };

    /**
     *Class to calculate the input variables for the BDT based AK8 W selection
     */
    class BDTDijetInputCalculator : public MVAInputCalculator
    {
    private:
        int var_fj_sdmass_, var_fj_tau21_, var_fj_ptDR_, var_fj_rel_ptdiff_, var_sj1_ptD_, var_sj1_axis1_, var_sj1_mult_, var_sj2_ptD_, var_sj2_axis1_, var_sj2_mult_, var_sjmax_csv_, var_sd_n2_;

    public:
        BDTDijetInputCalculator();
        void mapVars(const std::vector<std::string>&);
        bool calculateVars(const TopObject&, int);
        bool checkCand(const TopObject&);
    };

    /**
     *Class to calculate the input variables for the AK4 based resolved top catagory
     */
    class TrijetInputCalculator : public MVAInputCalculator
    {
    private:
        //Nconstituents
        static constexpr int NCONST = 3;
        //Get top candidate variables
        int cand_pt_;
        int cand_p_;
        int cand_eta_;
        int cand_phi_;
        int cand_m_;
        int cand_dRMax_;
        int cand_dThetaMin_;
        int cand_dThetaMax_;

        int j_m_lab_[NCONST];
        int j_CSV_lab_[NCONST];
        int j_QGL_lab_[NCONST];
        int j_qgMult_lab_[NCONST];
        int j_qgPtD_lab_[NCONST];
        int j_qgAxis1_lab_[NCONST];
        int j_qgAxis2_lab_[NCONST];
        int dR12_lab_[NCONST];
        int dR12_3_lab_[NCONST];
        int j12_m_lab_[NCONST];

        int dRPtTop_;
        int dRPtW_;
        int sd_n2_;

        int j_p_[NCONST];
        int j_p_top_[NCONST];
        int j_theta_top_[NCONST];
        int j_phi_top_[NCONST];
        int j_phi_lab_[NCONST];
        int j_eta_lab_[NCONST];
        int j_pt_lab_[NCONST];
        int j_m_[NCONST];
        int j_CSV_[NCONST];
        int j_QGL_[NCONST];
        int j_recoJetsJecScaleRawToFull_[NCONST];
        int j_qgLikelihood_[NCONST];
        int j_qgPtD_[NCONST];
        int j_qgAxis1_[NCONST];
        int j_qgAxis2_[NCONST];
        int j_recoJetschargedHadronEnergyFraction_[NCONST];
        int j_recoJetschargedEmEnergyFraction_[NCONST];
        int j_recoJetsneutralEmEnergyFraction_[NCONST];
        int j_recoJetsmuonEnergyFraction_[NCONST];
        int j_recoJetsHFHadronEnergyFraction_[NCONST];
        int j_recoJetsHFEMEnergyFraction_[NCONST];
        int j_recoJetsneutralEnergyFraction_[NCONST];
        int j_PhotonEnergyFraction_[NCONST];
        int j_ElectronEnergyFraction_[NCONST];
        int j_ChargedHadronMultiplicity_[NCONST];
        int j_NeutralHadronMultiplicity_[NCONST];
        int j_PhotonMultiplicity_[NCONST];
        int j_ElectronMultiplicity_[NCONST];
        int j_MuonMultiplicity_[NCONST];
        int j_DeepCSVb_[NCONST];
        int j_DeepCSVc_[NCONST];
        int j_DeepCSVl_[NCONST];
        int j_DeepCSVbb_[NCONST];
        int j_DeepCSVcc_[NCONST];
        int j_DeepFlavorb_[NCONST];
        int j_DeepFlavorbb_[NCONST];
        int j_DeepFlavorlepb_[NCONST];
        int j_DeepFlavorc_[NCONST];
        int j_DeepFlavoruds_[NCONST];
        int j_DeepFlavorg_[NCONST];
        int j_CvsL_[NCONST];
        int j_CvsB_[NCONST];
        int j_CombinedSvtx_[NCONST];
        int j_JetProba_[NCONST];
        int j_JetBprob_[NCONST];
        int j_recoJetsBtag_[NCONST];
        int j_recoJetsCharge_[NCONST];
        int j_qgMult_[NCONST];
        int dTheta_[NCONST];
        int j12_m_[NCONST];

    public:
        TrijetInputCalculator();
        void mapVars(const std::vector<std::string>&);
        bool calculateVars(const TopObject&, int);
        bool checkCand(const TopObject&);
    };

    std::vector<std::string> getMVAVars();

    //Gen matching helper functions 
    ///Helper function to help find the hadronically decaying gen tops 
    std::vector<TLorentzVector> GetHadTopLVec(const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);

    ///Helper function to get the direct decay products of the gen tops
    std::vector<const TLorentzVector*> GetTopdauLVec(const TLorentzVector& top, const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);

}

#endif
