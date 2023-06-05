#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <random>
#include <fstream>

using namespace std;

#define MAX_FILENAME 64

// Structure that contain config info
typedef struct arguments_struct {

    int k;
    int dimension;

    char input_filename[MAX_FILENAME];  // Name of the output file
    char output_filename[MAX_FILENAME]; // Name of the input file

} arguments_t;

// Calculate the Euclidean distance between two points
long double distance(vector<long double>& a, vector<long double>& b) {
    long double dist = 0.0;
    for (int i = 0; i < a.size(); i++) {
        dist += pow(a[i] - b[i], 2);
    }
    return sqrt(dist);
}

// Execute K-means clustering algorythm
void kMeansClustering(vector<vector<long double>>& data, int k, vector<vector<long double>>& centroids, vector<int>& clusters) {
    int n = data.size();
    int dim = data[0].size();
    
    // Initialize centroids randomly
    mt19937 gen(time(0));
    uniform_int_distribution<> dist(0, n-1);
    for (int i = 0; i < k; i++) {
        int idx = dist(gen);
        centroids[i] = data[idx];
    }
    
    // Assign each point to the nearest Pi-cluster
    for (int i = 0; i < n; i++) {
        long double minDist = 5;
        int minIdx = 0;
        for (int j = 0; j < k; j++) {
            long double dist = distance(data[i], centroids[j]);
            if (dist < minDist) {
                minDist = dist;
                minIdx = j;
            }
        }
        clusters[i] = minIdx;
    }
    
    // Repeated until convergence
    bool changed = true;
    int iter = 0;
    while (changed && iter < 100) {
        // Calculate new centroids
        vector<int> counts(k, 0);
        vector<vector<long double>> newCentroids(k, vector<long double>(dim, 0.0));
        for (int i = 0; i < n; i++) {
            int cluster = clusters[i];
            counts[cluster]++;
            for (int j = 0; j < dim; j++) {
                newCentroids[cluster][j] += data[i][j];
            }
        }

        for (int i = 0; i < k; i++) {
            if (counts[i] > 0) {
                for (int j = 0; j < dim; j++) {
                    newCentroids[i][j] /= counts[i];
                }
            }
        }
        
        // Assign each point to the nearest Pi-cluster
        changed = false;
        for (int i = 0; i < n; i++) {
            long double minDist = 5;
            int minIdx = 0;
            for (int j = 0; j < k; j++) {
                long double dist = distance(data[i], newCentroids[j]);
                if (dist < minDist) {
                    minDist = dist;
                    minIdx = j;
                }
            }
            if (minIdx != clusters[i]) {
                changed = true;
                clusters[i] = minIdx;
            }
        }
        centroids = newCentroids;
        iter++;
    }
}


    int main() {

        arguments_t args;

        // Read config
        std::ifstream conf("Kmeans_config.txt");
        conf >> args.dimension >> args.k >> args.input_filename >> args.output_filename;
        conf.close();

        // Open the input file
        std::ifstream inputFile(args.input_filename);

        // Create a vector that contains rawdata
        std::vector<std::vector<long double>> data;

        // Number of iterations done on input
        int s = 0;

        // Read the contents of the file
        while (!inputFile.eof()){

            // Read values in add them to vector "rawdata"
            std::vector<long double> point;
            for (int i = 0; i < args.dimension; ++i){

                long double coordinate;
                inputFile >> coordinate;
                point.push_back(coordinate);
            }

            // Program will ignore empty line at the end
            data.push_back(point);
            ++s;
        }
        data.pop_back();
        // Close the input file
        inputFile.close();

//        for (int i = 0; i < size(data); i++){
//            for (int j = 0; j < size(data[0]); j++){
//                cout << data[i][j] << " ";
//            }
//        }

        cout << "Data read, starting clustering" << endl;

        // Centroids
		vector<vector<long double>> centroids(args.k, vector<long double>(2));

		// Cluster	
		vector<int> clusters(data.size()); 
		
		// Measures the processing time
		clock_t start = clock();
		
		// Execute the K-means clustering algorithm
		kMeansClustering(data, args.k, centroids, clusters);

        cout << "Clusters found" << endl;

        ofstream out(args.output_filename);
        for (int i = 0; i < size(data); i++){
            out << clusters[i];
            for (int j = 0; j < args.dimension; j++) {
//                cout << clusters[i] << " " << data[i][j] << " " << data[i][j+1] << endl;
                out << " " << data[i][j];
            }
            out << endl;
        }
        out.close();

        // Calculate the processing time
        clock_t end = clock();
        double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;

        cout << "Finished clustering of " << args.output_filename << endl;
        cout << "Processing time: " << elapsed_secs << " seconds, have a nice day" << endl;

        return 0;
	}
