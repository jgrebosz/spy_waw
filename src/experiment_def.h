//---------------------------------------------------------------
// here are definitions for particular experiment

#ifndef EXPDEF_H
#define EXPDEF_H



// kind of betafield but by matrix  beta vs energy doppler
#define BETA_ANALOG

#define HOW_MANY_DANTE_PARAMETERS   40

// DO NOT CHANGE HERE, you may change 8 lines below this
#define RISING_STOPPED_BEAM_CAMPAIGN     1000
#define RISING_FAST_BEAM_CAMPAIGN        1100
#define G_FACTOR_OCTOBER_2005           1200
#define RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN     1300
#define RISING_ACTIVE_STOPPER_100TIN                       1400
#define RISING_ACTIVE_STOPPER_APRIL_2008            1500
#define RISING_ACTIVE_STOPPER_JULY_2008            1600
#define GALILEO_PROTOTYPE      2010

//#define CORR_ADC_PROFILE

#define STEPHANE_ASK_FOR_ORIGINAL_TIME_IN_THE_ROOT_TREE true
//==========================================================
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// HERE YOU MAKE CHANGES (you make the decision) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// uncomment the one line which coresponds to your experimment and  coment the other lines

//#define CURRENT_EXPERIMENT_TYPE       G_FACTOR_OCTOBER_2005
//#define CURRENT_EXPERIMENT_TYPE       RISING_FAST_BEAM_CAMPAIGN
//#define CURRENT_EXPERIMENT_TYPE       RISING_STOPPED_BEAM_CAMPAIGN
// #define CURRENT_EXPERIMENT_TYPE       RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN
#define CURRENT_EXPERIMENT_TYPE       GALILEO_PROTOTYPE

//#define CURRENT_EXPERIMENT_TYPE       RISING_ACTIVE_STOPPER_100TIN
//#define CURRENT_EXPERIMENT_TYPE       RISING_ACTIVE_STOPPER_APRIL_2008
//#define CURRENT_EXPERIMENT_TYPE       RISING_ACTIVE_STOPPER_JULY_2008
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



#define NEED_DOPPLER_SPECTRA  true
#define DOPPLER_CORRECTION_NEEDED
// Now details:
//####################################################################

constexpr unsigned int HOW_MANY_NEDA  = 100;
constexpr unsigned int HOW_MANY_EAGLE =  32+100;
constexpr unsigned int HOW_MANY_DSSD =  {16+32};

constexpr int HOW_MANY_DIAMANT {79};
//#################################################################
#define DIAMANT_PRESENT  0

#endif  //guardian
