#include <stdio.h>
#include <time.h>
#include <math.h>
#include "mt19937.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NDIM 3
#define N 400

/* Initialization variables */
const int mc_steps = 100000;
const int output_steps = 100;
const double packing_fraction = 0.6;
const double diameter = 1.0;
const double delta  = 0.1;
/* Volume change -deltaV, delta V */
const double deltaV = 2.0;
/* Reduced pressure \beta P */
const double betaP = 3.0;
const char* init_filename = "fcc.dat";

/* Simulation variables */
int n_particles = 0;
double radius;
double particle_volume;
double r[N][NDIM];
double box[NDIM];


/* Functions */
int change_volume(void){
    /*--------- Your code goes here -----------*/
    return 0;
}

void read_data(void){
    FILE* fp = fopen(init_filename, "r");
    int n, d;
    for(d = 0; d < NDIM; ++d) fscanf(fp, "%lf\t", &box[d]);
    fscanf(fp, "%d\n", &n_particles);
    for(n = 0; n < n_particles; ++n){
        for(d = 0; d < NDIM; ++d) fscanf(fp, "%lf\t", &r[n][d]);
    }
    fclose(fp);
}

int move_particle(void){
    int rpid = n_particles * dsfmt_genrand();


    double new_pos[NDIM];
    int d;
    for(d = 0; d < NDIM; ++d){
        new_pos[d] = r[rpid][d] + delta * (2.0 * dsfmt_genrand() - 1.0) + box[d];
        new_pos[d] -= (int)(new_pos[d] / box[d]) * box[d];
    }

    int n;
    for(n = 0; n < n_particles; ++n){
        if(n == rpid) continue;
        double dist = 0.0;
        for(d = 0; d < NDIM; ++d){
            double min_d = new_pos[d] - r[n][d];
            min_d -= (int)(2.0 * min_d / box[d]) * box[d];
            dist += min_d * min_d;
        }
        if(dist <= diameter * diameter) return 0;
    }

    for(d = 0; d < NDIM; ++d) r[rpid][d] = new_pos[d];

    return 1;
}

void write_data(int step){
    char buffer[128];
    sprintf(buffer, "coords_step%08d.dat", step);
    FILE* fp = fopen(buffer, "w");
    int d, n;
    for(d = 0; d < NDIM; ++d) fprintf(fp, "%lf\t", box[d]);
    fprintf(fp, "\n%d\n", n_particles);
    for(n = 0; n < n_particles; ++n){
        for(d = 0; d < NDIM; ++d) fprintf(fp, "%lf\t", r[n][d]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void set_packing_fraction(void){
    double volume = 1.0;
    int d, n;
    for(d = 0; d < NDIM; ++d) volume *= box[d];

    double target_volume = (n_particles * particle_volume) / packing_fraction;
    double scale_factor = pow(target_volume / volume, 1.0 / NDIM);

    for(n = 0; n < n_particles; ++n){
        for(d = 0; d < NDIM; ++d) r[n][d] *= scale_factor;
    }
    for(d = 0; d < NDIM; ++d) box[d] *= scale_factor;
}

int main(int argc, char* argv[]){

    radius = 0.5 * diameter;

    if(NDIM == 3) particle_volume = M_PI * pow(diameter, 3.0) / 6.0;
    else if(NDIM == 2) particle_volume = M_PI * pow(radius, 2.0);
    else{
        printf("Number of dimensions NDIM = %d, not supported.", NDIM);
        return 0;
    }

    read_data();

    if(n_particles == 0){
        printf("Error: Number of particles, n_particles = 0.\n");
        return 0;
    }

    set_packing_fraction();

    dsfmt_seed(time(nullptr));

    int move_accepted = 0;
    int vol_accepted = 0;
    int step, n;
    for(step = 0; step < mc_steps; ++step){
        for(n = 0; n < n_particles; ++n){
            move_accepted += move_particle();
        }
        vol_accepted += change_volume();

        if(step % output_steps == 0){
            printf("Step %d. Volume: %lf.\n Move acceptance: %lf, Volume acceptance: %lf.\n", step, box[0] * box[1] * box[2], (double)move_accepted / (n_particles * output_steps), (double)vol_accepted /  output_steps);
            move_accepted = 0;
            vol_accepted = 0;
            write_data(step);
        }
    }

    return 0;
}
