//------------ NEUTRONS ------------------------------

ulong nneda = times_of_crystals.size();
ulong k_list= 0;
vector<NEUTRON_DATA> neutron;   //[MAX_NEDA];
neutron.resize(nneda);
//memset(neutron,0,sizeof(neutron));


for (uint i_list=0; i_list < nneda; i_list++) {
    TNEDA_crystal * detptr = detector_neda[times_of_crystals[i_list].key];

    if (time_ref_type==1) {   // is a gamma ?

        // if it is inside of some polygon???

        if (neutron_cclf_cut_polygon_ptr &&
                neutron_cclf_cut_polygon_ptr->Test(detptr->give_psdlf(),       // x
                                                   detptr->give_real_time()) == true)
        {


            neutron[k_list].id = detptr->give_det_name_nr();
            neutron[k_list].i_list = i_list;
            neutron[k_list].rtime = detptr->give_real_time();
            neutron[k_list].cclf   =  detptr->give_psdlf();
            neutron[k_list].ccfpga =  detptr->give_psdcaen();
            neutron[k_list].light =  detptr->give_light();
            neutron[k_list].ban_result = 1;

            k_list++;
        }
    }
    else if (time_ref_type==2)
    {
        if (neutron_second_t2_cut_polygon_ptr &&      // the first neutron is in this cut as well
                neutron_second_t2_cut_polygon_ptr->
                Test(detptr->give_psdlf(),       // x
                     detptr->give_real_time())     // y
                )
        {

            neutron[k_list].id = detptr->give_det_name_nr();
            neutron[k_list].i_list = i_list;
            neutron[k_list].rtime = detptr->give_real_time();
            neutron[k_list].cclf   =  detptr->give_psdlf();
            neutron[k_list].ccfpga =  detptr->give_psdcaen();
            neutron[k_list].light =  detptr->give_light();
            neutron[k_list].ban_result = 2;
            k_list++;
        }
    }
}
int nr_neutrons=k_list;

int nr_scatters = 0;
int nr_separated = 0;

for (long i_list=0; i_list < nr_neutrons-1; i_list++) {
    int id1 = neutron[i_list].id;
    double z1 = neutron[i_list].light;
    double t1 = neutron[i_list].rtime;

    for (ulong  j_list=i_list+1; j_list < nr_neutrons; j_list++) {
        int   id2 = neutron[j_list].id;
        double z2 = neutron[j_list].light;
        double t2 = neutron[j_list].rtime;
        double dt;
        if (z1>=z2) {
            dt  = t2 - t1;
        }
        else {
            dt =  t1 - t2;
        }
        // this is a matrix dr - distance between two hits on neda detectors
        // versus time dt between them. It is to eliminate
        // events where the same neutron was
        TVector3  dv = neda_xyz[id1] - neda_xyz[id2];
        double dr = (double)dv.Mag();
        spec_hNe_dt_vs_dr->increment(dr,dt);


        // checking if it is in a triangled polygon gate

        if(triangel_polygon_ptr && triangel_polygon_ptr->Test(dr,dt))
        {
            nr_separated++;
        }
        else {
            nr_scatters++;
        }

    }
}
 nr_of_clean_neutrons = nr_neutrons - nr_scatters;
if (nr_neutrons>0 && nr_of_clean_neutrons < 1) {
    nr_of_clean_neutrons = 1;
}
//    if(nr_neutrons > 0)
//    cout << "n_neutrons_clean " << nr_of_clean_neutrons
//         << ", because n_neutrons= " << nr_neutrons
//         << "  , scatters =" << nr_scatters
//         << endl;


}
