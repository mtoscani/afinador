#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

#endif // HEADER_H_INCLUDED


#define LENGTH 256
#define FHT_N 256
#define LOG_OUT 1

// #include <FHT.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>

int rawData[LENGTH];
uint8_t init_array[] = {255,0,0,0};


std::vector<double> load_data(
                              std::string filename,
                              uint32_t length
                              )
{
    std::vector<double> output(length, 0);
    std::ifstream input(filename);

    for (uint32_t i = 0; i < length; i++) {
        input >> output[i];
    };

    input.close();

    return output;
};


void save_data(
               std::string filename,
               std::vector<double> data
               )
{
    std::ofstream output(filename, std::ofstream::out);
    int i = 0;

    for (i; i < data.size()-1; i++)
        output << data[i] << std::endl;

    output << data[i];
    output.close();
};


void float2bytes(float val, char* bytes_array){

    union {
        float float_variable;
        char temp_array[4];
    } u;

    u.float_variable = val;
    memcpy(bytes_array, u.temp_array, 4);
};


void floatarr2bytearr(
                      float input[],
                      int len,
                      char output[])
{
  for (int i=0; i < len; i++)
      float2bytes(input[i], &output[i*4]);
};


int find_max(
             uint8_t input[],
             int len, int offset
             )
{
    int c;
    int location = offset;
    int maximum = input[offset];

    for (c = offset+1; c < len; c++)
    {
        if (input[c] > maximum)
        {
           maximum  = input[c];
           location = c+1;
        };
    };

    return location;
};



int find_first_max(
                   std::vector<double> data,
                   double threshold_ratio
                   )
{
    uint32_t idx = 1;
    uint32_t max_idx = 0;
    double max_value = data[0];
    bool stop = false;

    for (idx; idx < data.size()-1; idx++)
    {
        // Encuentra un pico
        if (data[idx]-data[idx-1] > 0 && data[idx]-data[idx+1] > 0)
        {
            // Se fija si el valor de pico es mayor a threshold_ratio * max_value
            if (data[idx] > threshold_ratio * max_value) {

                max_idx = idx;
                max_value = data[idx];

                stop = true;
            }
        }

        if (stop) break;
   }

   return max_idx;
}


/*
int find_first_max2(
                   std::vector<double> data,
                   double prominence
                   )
{
   uint32_t idx = 1;
   uint32_t max_idx = 0;
   double max_value = data[0];
   double min_value = data[0];
   double prev_val = 0;
   double next_val = 0;
   bool stop = false;

   for (idx; idx < data.size()-1; idx++)
   {
        if (data[idx] < min_value) min_value = data[idx];

        if (data[idx]-data[idx-1] > 0 && data[idx]-data[idx+1] > 0)
        {
            if (data[idx] - min_value > prominence) {

                max_idx = idx;
                max_value = data[idx];

                for (uint32_t idx2 = idx, idx2 < data.size(), idx2++)
                    if (data[max_idx] - data[idx2] > prominence) { };

                stop = true;

            }

        }

        if (stop) break;
   }

   return max_idx;
}
*/


double compute_mean(int* input, int len)
{
    double mean;        // Computed mean value to be returned
    int i;           // Loop counter

    // Loop to compute mean
    mean = 0.0;
    for (i=0; i<len; i++)
        mean += ((double)input[i]) / len;

    return(mean);
};


double compute_mean(std::vector<double> input)
{
    double mean = 0.0;        // Computed mean value to be returned
    int i = 0;           // Loop counter

    for (i=0; i<input.size(); i++)
        mean += input[i] / input.size();

    return(mean);
};


void compute_autocorrelation(int* input, int len, double* output)
{
    for(int lag=0; lag < len; lag++)
    {
        double mean = compute_mean(rawData, len);

        // Loop to compute autovariance
        output[lag] = 0.0;
        for (int i=0; i<(len-lag); i++)
            output[lag] += (((double) input[i]/mean - 1) * ((double) input[i+lag]/mean - 1));
        output[lag] = output[lag] / (len - lag);
    };
};


std::vector<double> compute_autocorrelation(std::vector<double> input)
{
    uint32_t len = input.size();
    std::vector<double> output (len, 0);

    for(uint32_t lag=0; lag < len; lag++)
    {
        double mean = compute_mean(input);

        for (int i=0; i<(len-lag); i++)
            output[lag] += ((input[i]/mean - 1) * (input[i+lag]/mean - 1));
        output[lag] = output[lag] / (len - lag);
    };

    return output;
};


uint32_t detect_frequency(
                          std::vector<double> input,
                          double threshold_ratio
                          )
{
    uint32_t len = input.size();
    std::vector<double> corr (len, 0);

    bool stop = false;
    double max_value = 0;
    uint32_t max_idx = 0;
    double mean = compute_mean(input);

    // Iteración para distintos lags
    for(uint32_t lag=0; lag < len; lag++)
    {
        // Computa correlación para el lag actual
        for (uint32_t i=0; i<(len-lag); i++)
            corr[lag] += ((input[i]/mean - 1) * (input[i+lag]/mean - 1));
        corr[lag] = corr[lag] / (len - lag);

        // Detecta si el lag anterior es un pico
        if (lag > 1) {
            if (corr[lag-1]-corr[lag-2] > 0 && corr[lag-1]-corr[lag] > 0)
            {
                // Se fija si el valor de pico es mayor a threshold_ratio * corr[0]
                if (corr[lag-1] > threshold_ratio * corr[0]) {

                    max_idx = lag-1;
                    max_value = corr[lag-1];

                    stop = true;
                }
            }
        }

        if (stop) break;
    };

    return max_idx;
};




/*
void loop() {

    double autocorr[LENGTH];
    int max_idx;
    int freq_offset = 5;

    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < LENGTH ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      rawData[i] = k; // put real data into bins
      fht_input[i] = k; // put real data into bins
    }

    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei();

//    compute_autocorrelation(rawData, LENGTH, autocorr);
    max_idx = find_max(fht_log_out, LENGTH/2, freq_offset);

//    Serial.println("Hola mundo");
//    Serial.println(max_idx);


//    for (int i=0; i<LENGTH/2; i++){
//      Serial.print(fht_log_out[i]);
//      Serial.print(",");
//    };
//    Serial.println("");
      Serial.println(max_idx);


//    byte outbyte[LENGTH*sizeof(float)];
//    floatarr2bytearr(autocorr, LENGTH, outbyte);
//    Serial.write(init_array, sizeof(init_array));
//    Serial.write(outbyte, sizeof(outbyte));

}

//    for(int lag=0; lag < LENGTH; lag++)
//      {
//        int      i;           // Loop counter
//
//        double mean;
//        mean = compute_mean(rawData);
//
//        // Loop to compute autovariance
//        autocv[lag] = 0.0;
//        for (i=0; i<(LENGTH - lag); i++)
//          autocv[lag] += ((rawData[i] - mean) * (rawData[i+lag] - mean));
//        autocv[lag] = (1.0 / (LENGTH - lag)) * autocv[lag];
//      }
*/
