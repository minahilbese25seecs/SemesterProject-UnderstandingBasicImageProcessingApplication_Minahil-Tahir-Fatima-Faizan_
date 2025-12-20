#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include<vector>
using namespace std;
void gaussianBlurPass(vector<unsigned char>& pixels, int w, int h) {

    float kernel[5][5] = {
        { 1 / 273.0f,  4 / 273.0f,  7 / 273.0f,  4 / 273.0f,  1 / 273.0f },
        { 4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f,  4 / 273.0f },
        { 7 / 273.0f, 26 / 273.0f, 41 / 273.0f, 26 / 273.0f,  7 / 273.0f },
        { 4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f,  4 / 273.0f },
        { 1 / 273.0f,  4 / 273.0f,  7 / 273.0f,  4 / 273.0f,  1 / 273.0f }
    };

    vector<unsigned char> out(pixels.size());

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

            float rSum = 0, gSum = 0, bSum = 0;

            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {

                    int nx = x + kx;
                    int ny = y + ky;

                    if (nx < 0 || nx >= w || ny < 0 || ny >= h)
                        continue;

                    float weight = kernel[ky + 2][kx + 2];
                    int idx = (ny * w + nx) * 3;

                    rSum += pixels[idx] * weight;
                    gSum += pixels[idx + 1] * weight;
                    bSum += pixels[idx + 2] * weight;
                }
            }

            int outIndex = (y * w + x) * 3;

            out[outIndex] = (unsigned char)rSum;
            out[outIndex + 1] = (unsigned char)gSum;
            out[outIndex + 2] = (unsigned char)bSum;
        }
    }

    pixels = out;  // IMPORTANT: new blurred image becomes input for next pass
}

void downscale(vector<unsigned char>& pixels, int w, int h, int scale,
    vector<unsigned char>& small, int& smallW, int& smallH)
{
    smallW = w / scale;
    smallH = h / scale;

    small.resize(smallW * smallH * 3);

    for (int y = 0; y < smallH; y++) {
        for (int x = 0; x < smallW; x++) {

            int srcX = x * scale;
            int srcY = y * scale;

            // decreasing the size of the photo
            if (srcX >= w) srcX = w - 1;
            if (srcY >= h) srcY = h - 1;

            int srcIdx = (srcY * w + srcX) * 3;
            int dstIdx = (y * smallW + x) * 3;

            small[dstIdx] = pixels[srcIdx];
            small[dstIdx + 1] = pixels[srcIdx + 1];
            small[dstIdx + 2] = pixels[srcIdx + 2];
        }
    }
}
void upscale(const vector<unsigned char>& small, int smallW, int smallH,
    int scale, vector<unsigned char>& big, int origW, int origH)
{
    big.resize(origW * origH * 3);

    for (int y = 0; y < origH; y++) {
        for (int x = 0; x < origW; x++) {

            int srcX = x / scale;
            int srcY = y / scale;

            //increasing the size of the photo
            if (srcX >= smallW) srcX = smallW - 1;
            if (srcY >= smallH) srcY = smallH - 1;

            int srcIdx = (srcY * smallW + srcX) * 3;
            int dstIdx = (y * origW + x) * 3;

            big[dstIdx] = small[srcIdx];
            big[dstIdx + 1] = small[srcIdx + 1];
            big[dstIdx + 2] = small[srcIdx + 2];
        }
    }
}
void sharpen(vector<unsigned char>& pixels, int w, int h)
{
    vector<unsigned char> out(pixels.size());

    int kernel[3][3] = {
        { 0, -1,  0 },
        {-1,  5, -1 },  // medium level sharpening
        { 0, -1,  0 }
    };

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {

            float r = 0, g = 0, b = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {

                    int nx = x + kx;
                    int ny = y + ky;

                    int idx = (ny * w + nx) * 3;
                    int weight = kernel[ky + 1][kx + 1];

                    r += pixels[idx] * weight;
                    g += pixels[idx + 1] * weight;
                    b += pixels[idx + 2] * weight;
                }
            }

            int outIdx = (y * w + x) * 3;

            out[outIdx] = min(255, max(0, (int)r));
            out[outIdx + 1] = min(255, max(0, (int)g));
            out[outIdx + 2] = min(255, max(0, (int)b));
        }
    }

    pixels = out;
}




using namespace std;
int main() {
    //-----Do not touch-----
    char userProfile[512];//by doing so this code automatically saves the desktop location on your device
    size_t len = 0;
    errno_t err = getenv_s(&len, userProfile, sizeof(userProfile), "USERPROFILE");

    if (err != 0 || len == 0) {
        cout << "ERROR: Could not detect USERPROFILE environment variable." << endl;
        return 1;
    }

    string home(userProfile);

    //this will automactically build a path for user's desktop location
    string userFolder = home + "\\Desktop\\ImageCraft\\User's photos\\";
    string filteredFolder = home + "\\Desktop\\ImageCraft\\Filtered photos\\";

    // This application comes with predefined input and output folders
    cout << "Input folder:  " << userFolder << endl;
    cout << "Output folder: " << filteredFolder << endl;

    //user can enter the name of input and output photo to give a sense of freedom
    string inFileName, outFileName;

    cout << "Enter INPUT filename (example: input.ppm): ";
    cin >> inFileName;

    cout << "Enter OUTPUT filename (example: result.ppm): ";
    cin >> outFileName;

    // full paths
    string inputPath = userFolder + inFileName;
    string outputPath = filteredFolder + outFileName;

    // opening the files
    ifstream image(inputPath, ios::binary);
    ofstream newimage(outputPath, ios::binary);
    //------  ---------

    int choice; //Please make it a switch statement Fatima
    cout << "1. Blue Filter" << endl;
    cout << "2. Red Filter" << endl;
    cout << "3. Green Filter" << endl;
    cout << "4. Custom RGB Filter" << endl;
    cout << "5. Blur Filter" << endl;
    cout << "6. Sharpen Filter" << endl;
    cout << "Enter choice: ";
    cin >> choice;
    int addR = 0, addG = 0, addB = 0;
    if (choice == 4) {
        cout << "--Custom Filter Selected--" << endl;
        cout << "Enter Red Adjustment(-255 to 255): " << endl;
        cin >> addR;                      //MAKE SURE USER ENTERS VALUE BETWEEN -255 AND 255 do same for the choice above Fatima
        cout << "Enter Green Adjustment(-255 to 255): " << endl;
        cin >> addG;
        cout << "Enter Blue Adjustment(-255 to 255): " << endl;
        cin >> addB;

    }
    if (!image) {
        cout << "ERROR: Cannot open file: " << inputPath << endl;
        cout << "Make sure the file exists in the 'User's photos' folder." << endl;
        return 1;
    }


    cout << "Processing....This may take a while." << endl;

    string type = "", width = "", height = "", RGB = "";
    image >> type;
    image >> width;
    image >> height;
    image >> RGB;
    image.get();

    // stoi: converting string to integers
    int w = stoi(width);
    int h = stoi(height);
    int maxVal = stoi(RGB);
    //saving headers of input image
    newimage << "P6" << endl;
    newimage << width << " " << height << endl;
    newimage << RGB << endl;
    //collecting data from input image
    vector<unsigned char> pixels(w * h * 3);//w*h*3 are total bytes
    image.read(reinterpret_cast<char*>(pixels.data()), pixels.size());
    if (choice == 6) {
        sharpen(pixels, w, h);
        cout << "Sharpening applied!" << endl;
    }
    //Fatima add points by increments in each choice
    if (choice == 5) {
        if (choice == 5) {

            int scale = 10;
            int origW = w;
            int origH = h;

            vector<unsigned char> small;
            int smallW, smallH;

            // 1. Firslty decrease the size of the photo.
            downscale(pixels, origW, origH, scale, small, smallW, smallH);

            // 2. Blur the small photo more than once for a stronger effect
            for (int i = 0; i < 5; i++)
                gaussianBlurPass(small, smallW, smallH);

            // 3. Increase the size of the photo.
            vector<unsigned char> big;
            upscale(small, smallW, smallH, scale, big, origW, origH);

            pixels = big;     // final blurred image
            w = origW;
            h = origH;

            
        }



    }//Fatima please form fucntions for the rgb filters including the custom one
    if (choice != 5) {
        for (size_t i = 0; i < pixels.size(); i += 3) {
            // pixels[i] = Red, pixels[i+1] = Green, pixels[i+2] = Blue
            unsigned char r = pixels[i];
            unsigned char g = pixels[i + 1];
            unsigned char b = pixels[i + 2];
            if (choice == 1) {
                // BLUE FILTER
                int newBlue = b + 100;
                if (newBlue > 255) {
                    pixels[i + 2] = 255;
                }//converting it back to bytes
                else {
                    pixels[i + 2] = static_cast<unsigned char>(newBlue);
                }
            }
            if (choice == 2) {
                // RED FILTER
                int newRed = r + 100;
                if (newRed > 255) {
                    pixels[i] = 255;
                }
                else {
                    pixels[i] = static_cast<unsigned char>(newRed);
                }
            }
            if (choice == 3) {
                // GREEN FILTER
                int newGreen = g + 50;
                if (newGreen > 255) {
                    pixels[i + 1] = 255;
                }
                else {
                    pixels[i + 1] = static_cast<unsigned char>(newGreen);
                }
            }
            if (choice == 4) {
                // CUSTOM FILTER
                int newRed = r + addR;
                int newGreen = g + addG;
                int newBlue = b + addB;

                if (newRed < 0)newRed = 0;
                if (newRed > 255)newRed = 255;
                if (newGreen < 0)newGreen = 0;
                if (newGreen > 255)newGreen = 255;
                if (newBlue < 0)newBlue = 0;
                if (newBlue > 255)newBlue = 255;
                pixels[i] = static_cast<unsigned char>(newRed);
                pixels[i + 1] = static_cast<unsigned char>(newGreen);
                pixels[i + 2] = static_cast<unsigned char>(newBlue);

            }
        }
    }
    //----- Do not touch------
    //storing all the changes in the new image
    newimage.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
    //closing the files that were opened through ifstream/ofstream
    image.close();
    newimage.close();
    //saving the picture to the fixed location
    cout << "Processed successfully.\nSaved to: " << outputPath << endl;
    //-----           -------
    return 0;
}