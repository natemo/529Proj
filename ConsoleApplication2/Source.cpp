#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cstdlib>

#define SIGNAL_LENGTH 500 //31
#define FILTER_ORDER 10
#define DESIRED_SIGNAL (cos(0.1 * i))

void lms_filter(double x, double w[], double d, double mu, double return_array[]);

int
main(void)
{
	FILE *output_fp;
	FILE *error_fp;
	FILE *desired_fp;
	FILE *input_fp;
	
	bool input_file = false;
	double read = 0.0;
	char read_string[100];

	output_fp = fopen("OUTPUT.csv", "w+");
	error_fp = fopen("ERROR.csv", "w+");
	desired_fp = fopen("DESIRED.csv", "w+");
	//Comment/uncomment these two lines if you have input data in a csv file
	input_fp = fopen("INPUT.csv", "r+");
	input_file = true;

	
	double x[SIGNAL_LENGTH] = { 0.0 };
	double d[SIGNAL_LENGTH] = { 0.0 };
	double y[SIGNAL_LENGTH] = { 0.0 };
	double w[FILTER_ORDER] = { 0.0 };

	double lms_return[2] = { 0.0, 0.0 };

	double mu = 0.1; //0.35 blows up, 0.3
	
	for (int i = 0; i < SIGNAL_LENGTH; i++)
	{
		y[i] = 0.0;
	}
	
	for (int i = 0; i < SIGNAL_LENGTH; i++)
	{
		if (!input_file)
		{
			x[i] = 1.0;
		}
		else
		{
			fgets(read_string, 100, input_fp);
			x[i] = atof(read_string);
		}
	}

	for (int i = 0; i < FILTER_ORDER; i++) {
		w[i] = 0.0;
	}

	for (int i = 0; i < SIGNAL_LENGTH; i++) {
		d[i] = DESIRED_SIGNAL;
		if (i < (SIGNAL_LENGTH - 1)) {
			fprintf(desired_fp, "%lf,\n", d[i]);
		}
		else
		{
			fprintf(desired_fp, "%lf", d[i]);
		}
	}

	for (int i = 0; i < SIGNAL_LENGTH; i++) {
		lms_filter(x[i], w, d[i], mu, lms_return);
		if (i < (SIGNAL_LENGTH - 1)) {
			fprintf(output_fp, "%lf,\n", lms_return[0]);
			fprintf(error_fp, "%lf,\n", lms_return[1]);
		}
		else
		{
			fprintf(output_fp, "%lf", lms_return[0]);
			fprintf(error_fp, "%lf", lms_return[1]);
		}
	}

	fclose(output_fp);
	fclose(error_fp);
	fclose(desired_fp);

	return 0;
}

/* LMS adaptive filter
 *
 * Author: Nathan Monson
 * 
 * Parameters:
 * x - most recent input sample
 * w - vector of filter term weights
 * d - value of the desired output at this index
 * mu - the step size (also referred to as the adaptation constant
 * return_array - results of computatation passed in this array - the filter output value in the first
 *				  index and the error value in the second index
 * 
 * This function implements the three main computational steps of an LMS filter:
 * (1) Update the input_buffer with the most recent sample and calculate filter output (y) by mult-accumulate
 * the input_buffer by the weights of the filter terms (w)
 * (2) Calculate the error term by subtracting the desired output value (d) from the calculated output value
 * (3) Update the weights vector (w) by adding the update term at each index
 */

void lms_filter(double x, double w[], double d, double mu, double return_array[])
{
	double error = 0.0;
	double y = 0.0;

	//stores the input(x) values that have previously been fed into the filter, most recent at index 0
	static double input_buffer[FILTER_ORDER]; 
	for (int i = 0; i < FILTER_ORDER; i++)
	{
		input_buffer[i + 1] = input_buffer[i]; //shift previous input values
	}
	
	input_buffer[0] = x; //push most recent input value into input_buffer
	
	for (int j = 0; j < FILTER_ORDER; j++)
	{
		y += input_buffer[j] * w[j]; // calculate filter output
	}

	return_array[0] = y;

	error = d - y; // calculate error term

	return_array[1] = error;

	for (int k = 0; k < FILTER_ORDER; k++)
	{
		w[k] = w[k] + (mu * error * input_buffer[k]); //update filter weight vector
	}
}