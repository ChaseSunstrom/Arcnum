#ifndef STARTER_INCLUDE_FILE_HPP
#define STARTER_INCLUDE_FILE_HPP

#include <iostream>
#include <complex>
#include <string>
#include <cstring>
#include <limits.h>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include <functional>
#include <locale>
#include <cctype>
#include <regex>
#include <fstream>
#include <cstdint>
#include <windows.h>
#include <chrono>

using namespace std;

#ifdef _WIN32
#define CLEAR_COMMAND "cls"
#else
#define CLEAR_COMMAND "clear"
#endif

const double planckTime = 5.391e-44; /*Seconds*/
const double planckLength = 1.616228369772e-35; /*meters*/
const double reducedPlanck = 1.05457187e-34; /*Joule seconds*/
const double planckConstant = 6.62607015e-34; /*Joule seconds*/
const double boltzmann = 1.380649e-23; /*Kilogram meter squared per second squared kelvin*/
const double gravitational = 6.6743e-11; /*Meters cubed per kilogram second squared*/
const double stefanBoltzmann = 5.67e-8; /*Watts per meter squared kelvin to the fourth*/
const double wienWavelength = 2.897771955e-3; /*kelvin meters*/
const double fineStructure = 7.29735257e-3;
const double champernowne = 0.123456789101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354; /*Just goes 123456...*/
const double omegaConstant = 0.5671432904097838729999686622103555497538157871865125081351310792230457930866845666932194469617522946; /*W(1)*/
const double EulerMascheroni = 0.577215664901532860606512090082402431042159335939923598805767234884867726777664670936947063291746749;
const double idealGas = 0.08205736608096060876445783215591213724670410156250; /*Atmosphere liters per mole kelvin*/
const double catalan = 0.915965594177219015054603514932384110774149374281672134266498119621763019776254769479356512926115106;
const double aperny = 1.202056903159594285399738161511449990764986292340498881792271555341838205786313090186455873609335258;
const double phi = 1.618033988749894848204586834365638117720309179805762862135448622705260462818902449707207204189391137;
const double euler = 2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427;
const double pi = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067;
const double khinchin = 2.685452001065306445309714835481795693820382293994462953051152345557218859537152002801141174931847698;
const double GelfondSchneider = 2.665144142690225188650297249873139848274211313714659492835979593364920446178705954867609180005196416;
const double rydberg = 1.0973731568e7; /*Reciprocal meters*/
const double lightSecond = 2.998e8; /*Meters per second*/
const double coloumb = 8.98755179e9; /*Newton square meters per coloumb squared*/
const double lightMinute = 1.799e10; /*Meters per minute*/
const double wienFrequency = 5.878926e10; /*Hertz per kelvin*/
const double astronomicalUnit = 1.496e11; /*Meters*/
const double lightHour = 1.079e12; /*Meters per hour*/
const double lightDay = 2.59e13; /*Meters per day*/
const double lightYear = 9.461e15; /*Meters per year*/
const double parsec = 3.086e16; /*Meters*/
const double megaparsec = 3.086e22; /*Meters*/
const double avagadro = 6.022e23; /*Number of atoms or molecules in a mole*/
const double gigaparsec = 3.086e25; /*Meters*/
const double tetraparsec = 3.086e28; /*Meters*/
const double petaparsec = 3.086e31; /*Meters*/
const double yottaparsec = 3.086e40; /*Meters*/

const int SVG_SIZE = 1000;

int row, column;
const int maxDecimals = 15;
double area;
double perimeter;
double eccentricity;

struct Point {
    double x;
    double y;
};

struct Point_3D {
    double x;
    double y;
    double z;
};

struct Point_4D {
    double x;
    double y;
    double z;
    double t;
};

using Function = long double (*)(long double);
using Complex_Number = complex<double>;
using Complex_Function = Complex_Number (*)(Complex_Number);
using Multi_Variable_Complex_Function = Complex_Number (*)(Complex_Number, Complex_Number);

/****************
----Overloads----
*****************/
// Overload the binary operator -
Complex_Number operator-(const Complex_Number& num1, const Complex_Number& num2) {
    return Complex_Number(num1.real() - num2.real(), num1.imag() - num2.imag());
}

// Overload the binary operator +
Complex_Number operator+(const Complex_Number& num1, const Complex_Number& num2) {
    return Complex_Number(num1.real() + num2.real(), num1.imag() + num2.imag());
}

// Overload the binary operator *
Complex_Number operator*(const Complex_Number& num1, const Complex_Number& num2) {
    return Complex_Number(
        num1.real() * num2.real() - num1.imag() * num2.imag(),
        num1.real() * num2.imag() + num1.imag() * num2.real()
    );
}


// Overload the binary operator /
Complex_Number operator/(const Complex_Number& num1, const Complex_Number& num2) {
    double denom = pow(num2.real(), 2) + pow(num2.imag(), 2);

    return Complex_Number(
        (num1.real() * num2.real() + num1.imag() * num2.imag()) / denom,
        (num1.imag() * num2.real() - num1.real() * num2.imag()) / denom
    );
}

/*********************
----Util functions----
**********************/
string to_string_with_precision(const double value, const int precision) {
    stringstream stream;
    stream << fixed << setprecision(precision) << value;
    string result = stream.str();
    // Remove trailing zeros
    result.erase(result.find_last_not_of('0') + 1, string::npos);
    // Remove trailing decimal point if all digits after it are zeros
    if (result.find('.') != string::npos && result.back() == '.')
        result.pop_back();
    return result;
}

class Stopwatch {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool running;

public:
    Stopwatch() : running(false) {}

    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        running = true;
    }

    void stop() {
        if (running) {
            end_time = std::chrono::high_resolution_clock::now();
            running = false;
        }
    }

    void reset() { running = false; }

    double elapsed_seconds() const {
        if (running) {
            auto elapsed = std::chrono::high_resolution_clock::now() - start_time;
            return std::chrono::duration<double>(elapsed).count();
        } else {
            auto elapsed = end_time - start_time;
            return std::chrono::duration<double>(elapsed).count();
        }
    }

    int hours() const {
        return static_cast<int>(elapsed_seconds()) / 3600;
    }

    int minutes() const {
        return static_cast<int>(elapsed_seconds()) / 60 % 60;
    }

    int seconds() const {
        return static_cast<int>(elapsed_seconds()) % 60;
    }

    int milliseconds() const {
        return static_cast<int>(elapsed_seconds() * 1000) % 1000;
    }

    void print_elapsed_time() const {
        cout << setfill('0') 
            << setw(2) << hours() << ":"
            << setw(2) << minutes() << ":"
            << setw(2) << seconds() << "."
            << setw(4) << milliseconds()
        << endl;
    }
};

/*****************
----Basic Math----
******************/
using op_func_t = Complex_Number (*)(Complex_Number, Complex_Number);

struct op_t {
    const char* inputName;
    const char* operatorName;
    op_func_t  op_func;
};

Complex_Number add(Complex_Number num1, Complex_Number num2) { return num1 + num2; }

Complex_Number multiply(Complex_Number num1, Complex_Number num2) { return num1 * num2; }

Complex_Number divide(Complex_Number num1, Complex_Number num2) {
    if (num2 == Complex_Number(0.0, 0.0)) { throw runtime_error("Error: division by zero"); }
    return num1 / num2;
}

Complex_Number subtract(Complex_Number num1, Complex_Number num2) { return num1 - num2; }

Complex_Number nthRoot(Complex_Number num1, Complex_Number num2) {
    if (num2 == Complex_Number(0.0, 0.0)) {
        cout << "Error: division by 0" << endl;
        return NAN;
    } else if (((num2.real() < 0 && num2.imag() == 0) || (num2.real() == 0 && num2.imag() < 0)) && (num1 == Complex_Number(0.0, 0.0))) {
        cout << "Error: division by 0" << endl;
        return NAN;
    }

    return pow(num1, 1.0 / num2);
}

Complex_Number nthPower(Complex_Number num1, Complex_Number num2) {
    if (num1 == Complex_Number(0.0, 0.0) && num2 == Complex_Number(0.0, 0.0)) {
        cout << "Error: two-sided limit does not exist" << endl;
        return NAN;
    } else if ((num1 == Complex_Number(0.0, 0.0)) && ((num2.real() < 0 && num2.imag() == 0) || (num2.real() == 0 && num2.imag() < 0))) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    return pow(num1, num2);
}

Complex_Number nthLog(Complex_Number num1, Complex_Number num2) {
    if (num2 == Complex_Number(0.0, 0.0)) {
        cout << "Error: log base 0" << endl;
        return NAN;
    }
    
    if (num1 == Complex_Number(0.0, 0.0)) {
        cout << "Error: argument error" << endl;
        return NAN;
    }

    return log(num1) / log(num2);
}

Complex_Number average(Complex_Number num1, Complex_Number num2) {
    Complex_Number sum = 0.0;
    double count = 0;

    for (double real = num1.real(); real <= num2.real(); real++) {
        for (double imag = num1.imag(); imag <= num2.imag(); imag++) {
            Complex_Number current(real, imag);
            sum += current;
            count++;
        }
    }

    return sum / count;
}

int compare_complex(const void *a, const void *b) {
    Complex_Number num1 = *((Complex_Number*)a);
    Complex_Number num2 = *((Complex_Number*)b);
    if (num1.real() != num2.real()) { return num1.real() < num2.real(); } else { return num1.imag() < num2.imag(); }    
}

Complex_Number determineMedian(Complex_Number num1, Complex_Number num2) {
    Complex_Number arr[] = {num1, num2};
    sort(arr, arr + 2, [](Complex_Number a, Complex_Number b) { return a.real() < b.real() || (a.real() == b.real() && a.imag() < b.imag()); });
    return arr[1];
}

Complex_Number median(Complex_Number num1, Complex_Number num2) {
    Complex_Number median;
    double real1 = num1.real();
    double imag1 = num1.imag();
    double real2 = num2.real();
    double imag2 = num2.imag();
    if (real1 == real2 && imag1 == imag2) { median = num1; } else { median = determineMedian(num1, num2); }
    return median;
}

Complex_Number mode(Complex_Number num1, Complex_Number num2) {
    Complex_Number mode = num1;
    int maxCount = 0;

    for (double real = num1.real(); real <= num2.real(); real++) {
        for (double imag = num1.imag(); imag <= num2.imag(); imag++) {
            Complex_Number current(real, imag);
            int count = 0;

            for (double r = num1.real(); r <= num2.real(); r++) {
                for (double i = num1.imag(); i <= num2.imag(); i++) {
                    Complex_Number temp(r, i);
                    if (temp == current) { count++; }
                }
            }

            if (count > maxCount) {
                maxCount = count;
                mode = current;
            }
        }
    }

    return mode;
}

Complex_Number factorial(Complex_Number num1, Complex_Number num2) {
    // Calculate num1^num2
    Complex_Number power_result = pow(num1 + 1, num2);

    // Get the real and imaginary parts
    double real_part = real(power_result);
    double imag_part = imag(power_result);

    // Calculate the gamma function of the real part
    double real_gamma = tgamma(real_part);

    // For the imaginary part, we use the property of gamma function:
    // gamma(x + yi) = gamma(x) * exp(-i * pi * y) * (sin(pi * y) + i * cos(pi * y))
    double real_sin = sin(pi * imag_part);
    double real_cos = cos(pi * imag_part);
    double imag_gamma_real = real_gamma * real_cos;
    double imag_gamma_imag = -real_gamma * real_sin;

    // Combine the real and imaginary parts
    return Complex_Number(imag_gamma_real, imag_gamma_imag);
}

double realFactorial(double max) {
    double result = 1;
    for (int i = 2; i <= max; ++i) {
        result *= i;
    }
    return result;
}

Complex_Number nPr(Complex_Number n, Complex_Number r) {
    if (n.imag() != 0 || r.imag() != 0) {
        cout << "nPr is not defined for complex numbers." << endl;
        return NAN;
    }

    return realFactorial(n.real()) / realFactorial(n.real() - r.real());
}

Complex_Number nCr(Complex_Number n, Complex_Number r) {
    if (n.imag() != 0 || r.real() != 0) {
        cout << "nCr is not defined for complex numbers." << endl;
        return NAN;
    }

    return realFactorial(n.real()) / (realFactorial(r.real()) * realFactorial(n.real() - r.real()));
}

Complex_Number ceiling(Complex_Number num1, Complex_Number num2) {
    if (num1.real() == 0 && num1.imag() == 0 && num2.real() == 0 && num2.imag() == 0) {
        cout << "Error: two-sided limit does not exist" << endl;
        return NAN;
    } else if ((num1.real() == 0 && num1.imag() == 0) && ((num2.real() < 0 && num2.imag() == 0) || (num2.real() == 0 && num2.imag() < 0))) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    Complex_Number result = pow(num1, num2);

    double realCeiling = ceil(result.real());
    double imagCeiling = ceil(result.imag());

    return Complex_Number(realCeiling, imagCeiling);
}

Complex_Number Floor(Complex_Number num1, Complex_Number num2) {
    if (num1.real() == 0 && num1.imag() == 0 && num2.real() == 0 && num2.imag() == 0) {
        cout << "Error: two-sided limit does not exist" << endl;
        return NAN;
    } else if ((num1.real() == 0 && num1.imag() == 0) && ((num2.real() < 0 && num2.imag() == 0) || (num2.real() == 0 && num2.imag() < 0))) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    Complex_Number result = pow(num1, num2);

    double realFloor = floor(result.real());
    double imagFloor = floor(result.imag());

    return Complex_Number(realFloor, imagFloor);
}

Complex_Number fractionalPart(Complex_Number num1, Complex_Number num2) {
    if (num1.real() == 0 && num1.imag() == 0 && num2.real() == 0 && num2.imag() == 0) {
        cout << "Error: two-sided limit does not exist" << endl;
        return NAN;
    } else if ((num1.real() == 0 && num1.imag() == 0) && ((num2.real() < 0 && num2.imag() == 0) || (num2.real() == 0 && num2.imag() < 0))) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    double real_part = pow(num1, num2).real() - floor((pow(num1, num2)).real());
    double imag_part = pow(num1, num2).imag() - floor((pow(num1, num2)).imag());
    
    return Complex_Number(real_part, imag_part);
}

Complex_Number Round(Complex_Number num1, Complex_Number num2) {
    double realPart = round((pow(num1, num2)).real());
    double imaginaryPart = round((pow(num1, num2)).imag());
    
    return Complex_Number(realPart, imaginaryPart);
}

Complex_Number absoluteValue(Complex_Number num1, Complex_Number num2) {
    if ((num1).real() == 0 && (num1).imag() == 0 && (num2).real() == 0 && (num2).imag() == 0) {
        cout << "Error: two-sided limit does not exist" << endl;
        return NAN;
    } else if (((num1).real() == 0 && (num1).imag() == 0) && (((num2).real() < 0 && (num2).imag() == 0) || ((num2).real() == 0 && num2.imag() < 0))) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    return abs(pow(num1, num2));
}

Complex_Number modulo(Complex_Number num1, Complex_Number num2) {
    double realPart = real(num1) - real(num2) * floor(real(num1) / real(num2));
    double imagPart = imag(num1) - imag(num2) * floor(imag(num1) / imag(num2));
    Complex_Number answer(realPart, imagPart);
    return answer;
}

Complex_Number getReal(Complex_Number num1, Complex_Number null) { return num1.real(); }

Complex_Number getImaginary(Complex_Number num1, Complex_Number null) { return num1.imag(); }

Complex_Number getArgument(Complex_Number num1, Complex_Number null) {
    if (num1.real() == 0) { return 90.0; }
    return atan(num1.imag() / num1.real());
}

Complex_Number getMagnitude(Complex_Number num1, Complex_Number num2) { return sqrt(pow(real(num1), 2) + pow(imag(num1), 2)); }

Complex_Number sine(Complex_Number num1, Complex_Number num2) {
    num1 = (num1 * pi) / 180.0;
    return pow(sin(num1), num2);
}

Complex_Number cosine(Complex_Number num1, Complex_Number num2) {
    num1 = (num1 * pi) / 180.0;
    return pow(cos(num1), num2);
}

Complex_Number tangent(Complex_Number num1, Complex_Number num2) {
    num1 = (num1 * pi) / 180.0;

    if (abs(cos(real(num1))) < 1e-10) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    return pow(tan(num1), num2);
}

Complex_Number cosecant(Complex_Number num1, Complex_Number num2) {
    num1 = (num1 * pi) / 180.0;
    Complex_Number sin_value = sin(num1);

    if (abs(sin_value) < 1e-10) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    return pow(1.0 / sin_value, num2);
}

Complex_Number secant(Complex_Number num1, Complex_Number num2) {
    num1 = (num1 * pi) / 180.0;
    Complex_Number cos_value = cos(num1);

    if (abs(cos_value) < 1e-10) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    return pow(1.0 / cos_value, num2);
}

Complex_Number cotangent(Complex_Number num1, Complex_Number num2) {
    num1 = (num1 * pi) / 180.0;

    if (abs(sin(num1)) < 1e-10) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    return pow(1.0 / tan(num1), num2);
}

Complex_Number arcsine(Complex_Number num1, Complex_Number num2) {
    Complex_Number result = asin(num1);
    result = result * 180.0 / pi;
    return result;
}

Complex_Number arccosine(Complex_Number num1, Complex_Number num2) {
    Complex_Number result = acos(num1);
    result = result * 180.0 / pi;
    return result;
}

Complex_Number arctangent(Complex_Number num1, Complex_Number num2) {
    Complex_Number result = atan(num1);
    result = result * 180.0 / pi;
    return result;
}

Complex_Number arccosecant(Complex_Number num1, Complex_Number num2) {
    if (num1 == Complex_Number(0.0, 0.0)) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    Complex_Number result = asin(1.0 / num1);
    result = result * 180.0 / pi;
    return result;
}

Complex_Number arcsecant(Complex_Number num1, Complex_Number num2) {
    if (num1 == Complex_Number(0.0, 0.0)) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    Complex_Number result = acos(1.0 / num1);
    result = result * 180.0 / pi;
    return result;
}

Complex_Number arccotangent(Complex_Number num1, Complex_Number num2) {
    if (num1 == Complex_Number(0.0, 0.0)) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    Complex_Number result = atan(1.0 / num1);
    result = result * 180.0 / pi;
    return result;
}

Complex_Number sineh(Complex_Number num1, Complex_Number num2) { return pow(sinh(num1), num2); }

Complex_Number cosineh(Complex_Number num1, Complex_Number num2) { return pow(cosh(num1), num2); }

Complex_Number tangenth(Complex_Number num1, Complex_Number num2) {
    if (abs(cosh(num1)) < 1e-10) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    return pow(tanh(num1), num2);
}

Complex_Number cosecanth(Complex_Number num1, Complex_Number num2) {
    num1 = (num1 * pi) / 180.0;

    if (abs(sinh(num1)) < 1e-10) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    return pow(1.0 / sinh(num1), num2);
}

Complex_Number secanth(Complex_Number num1, Complex_Number num2) {
    num1 = (num1 * pi) / 180.0;

    if (abs(cosh(num1)) < 1e-10) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    return pow(1.0 / cosh(num1), num2);
}

Complex_Number cotangenth(Complex_Number num1, Complex_Number num2) {
    num1 = (num1 * pi) / 180.0;

    if (abs(sinh(num1)) < 1e-10) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    return pow(1.0 / tanh(num1), num2);
}

Complex_Number arcsineh(Complex_Number num1, Complex_Number num2) {
    Complex_Number result = asinh(num1);
    result = result * (180 / pi);
    return result;
}

Complex_Number arccosineh(Complex_Number num1, Complex_Number num2) {
    Complex_Number result = acosh(num1);
    result = result * (180 / pi);
    return result;
}

Complex_Number arctangenth(Complex_Number num1, Complex_Number num2) {
    Complex_Number result = atanh(num1);
    result = result * (180 / pi);
    return result;
}

Complex_Number arccosecanth(Complex_Number num1, Complex_Number num2) {
    if (num1.real() == 0 && num1.imag() == 0) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    Complex_Number result = asinh(1.0 / num1);
    result = result * (180 / pi);
    return result;
}

Complex_Number arcsecanth(Complex_Number num1, Complex_Number num2) {
    if (num1.real() == 0 && num1.imag() == 0) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    Complex_Number result = acosh(1.0 / num1);
    result = result * (180 / pi);
    return result;
}

Complex_Number arccotangenth(Complex_Number num1, Complex_Number num2) {
    if (num1.real() == 0 && num1.imag() == 0) {
        cout << "Error: divide by 0" << endl;
        return NAN;
    }

    Complex_Number result = atanh(1.0 / num1);
    result = result * (180 / pi);
    return result;
}

Complex_Number randomInt(Complex_Number min, Complex_Number max) {
    srand(time(0));
    return rand() % (int)(max.real() - min.real() + 1) + min.real();
}

Complex_Number randomDouble(Complex_Number min, Complex_Number max) {
    static bool seeded = false;
    
    if (!seeded) {
        srand(time(nullptr)); // Seed the random number generator only once
        seeded = true;
    }

    return min.real() + (static_cast<double>(rand()) / RAND_MAX) * (max.real() - min.real());
}

Complex_Number fibonacci(Complex_Number x, Complex_Number null /*Unused here*/) { return (pow(phi, x) - pow(phi, -x) * cos(pi * x)) / sqrt(5.0); }

Complex_Number lucas(Complex_Number x, Complex_Number null /*Unused here*/) { return pow(-phi, -x.real()) + pow(phi, x.real()); }

const op_t operations[] = {
    //Basic functions
    {"add", "addition", add},
    {"multiply", "multiplication", multiply},
    {"divide", "division", divide},
    {"subtract", "subtraction", subtract},
    //Algebra functions
    {"nthRoot", "radical", nthRoot},
    {"nthPower", "exponential", nthPower},
    {"nthLog", "log", nthLog},
    //Number theory and statistics functions
    {"avg", "avgerage", average},
    {"med", "median", median},
    {"mode", "mode", mode},
    {"nPr", "nPr", nPr},
    {"nCr", "nCr", nCr},
    {"ceil", "ceiling", ceiling},
    {"floor", "floor", Floor},
    {"frac", "fractional part", fractionalPart},
    {"abs", "absolute value", absoluteValue},
    {"mod", "modulo", modulo},
    //Factorial stuff
    {"!", "factorial", factorial},
    //Imaginary stuff
    {"Im", "imaginary part", getImaginary},
    {"Re", "real part", getReal},
    {"arg", "argument", getArgument},
    {"mag", "magnitude", getMagnitude},
    //Trig functions
    {"sin", "sine", sine},
    {"cos", "cosine", cosine},
    {"tan", "tangent", tangent},
    {"csc", "cosecant", cosecant},
    {"sec", "secant", secant},
    {"cot", "cotangent", cotangent},
    //Inverse trig functions
    {"arcsin", "arcsine", arcsine},
    {"arccos", "arccosine", arccosine},
    {"arctan", "arctangent", arctangent},
    {"arccsc", "arccosecant", arccosecant},
    {"arcsec", "arcsecant", arcsecant},
    {"arccot", "arccotangent", arccotangent},
    //Hyperbolic trig functions
    {"sinh", "sinh", sineh},
    {"cosh", "cosh", cosineh},
    {"tanh", "tanh", tangenth},
    {"csch", "csch", cosecanth},
    {"sech", "sech", secanth},
    {"coth", "coth", cotangenth},
    //Hyperbolic inverse trig functions
    {"arcsinh", "arcsinh", arcsineh},
    {"arccosh", "arccosh", arccosineh},
    {"arctanh", "arctanh", arctangenth},
    {"arccsch", "arccsch", arccosecanth},
    {"arcsech", "arcsech", arcsecanth},
    {"arccoth", "arccoth", arccotangenth},
    //Other
    {"randInt", "random integer", randomInt},
    {"randDec", "random decimal", randomDouble},
    {"fibN", "fibonacchi equation", fibonacci},
    {"lucas", "lucas equation", lucas},
};

const size_t operations_supported = sizeof(operations) / sizeof(*operations);

/*****************
-----Calculus-----
******************/
long double limit(Function func, long double point, const string& direction = "both") {
    //Direction is left
    if (direction == "left") {
        long double left = func(point - 1e-10);
        return left;
    }
    
    //Direction is right
    if (direction == "right") {
        long double right = func(point + 1e-10);
        return right;
    }
    
    //Direction is both
    if (direction == "both") {
        long double left = func(point - 1e-10);
        long double right = func(point + 1e-10);
        return (left + right) / 2; 
    }    
}

double sum(Function f, double lowerBound, double upperBound) {
    double sum = 0;
    for (double i = lowerBound; i <= upperBound; i++) { sum += f(i); }
    return sum;
}

double product(Function f, double lowerBound, double upperBound) {
    double product = 1;
    for (double i = lowerBound; i <= upperBound; i++) { product *= f(i); }
    return product;
}

double integrate(Function f, double lowerBound, double upperBound) {
    int numSteps = 1e8;
    long double step_size = (upperBound - lowerBound) / numSteps;
    long double integral = 0;

    for (int i = 0; i < numSteps; i++) {
        long double x1 = lowerBound + i * step_size;
        long double x2 = lowerBound + (i + 1) * step_size;
        integral += (f(x1) + f(x2)) * step_size / 2;
    }

    return integral;
}

double derivative(Function func, long double point) {
    long double h = 1e-10; // Small value for h
    long double derivativeValue = (func(point + h) - func(point)) / h;
    return derivativeValue;
}

double realFactorial(int max) {
    double result = 1;
    for (int i = 2; i <= max; ++i) {
        result *= i;
    }
    return result;
}

double areaBetweenFunctions(Function func1, Function func2, double lowerBound, double upperBound) { return integrate(func1, lowerBound, upperBound) - integrate(func2, lowerBound, upperBound); }

/**************************
-----Special Functions-----
***************************/

/*********************
-----Pre Calculus-----
**********************/
//Vectors
Point rotatePoint2D(const Point& point, double angle) {
    double x = point.x;
    double y = point.y;
    double angle_rad = angle * pi / 180.0;
    double x_new = x * cos(angle_rad) - y * sin(angle_rad);
    double y_new = x * sin(angle_rad) + y * cos(angle_rad);
    return {x_new, y_new};
}

Point_3D rotatePoint3D(const Point_3D& point, double angle_x, double angle_y, double angle_z, const std::string& direction) {
    double x = point.x;
    double y = point.y;
    double z = point.z;

    double angle_x_rad = angle_x * pi / 180.0;
    double angle_y_rad = angle_y * pi / 180.0;
    double angle_z_rad = angle_z * pi / 180.0;

    for (char axis : direction) {
        if (axis == 'x' || axis == 'X') {
            double y_new = y * cos(angle_x_rad) - z * sin(angle_x_rad);
            double z_new = y * sin(angle_x_rad) + z * cos(angle_x_rad);
            y = y_new;
            z = z_new;
        } else if (axis == 'y' || axis == 'Y') {
            double x_new = x * cos(angle_y_rad) + z * sin(angle_y_rad);
            double z_new = -x * sin(angle_y_rad) + z * cos(angle_y_rad);
            x = x_new;
            z = z_new;
        } else if (axis == 'z' || axis == 'Z') {
            double x_new = x * cos(angle_z_rad) - y * sin(angle_z_rad);
            double y_new = x * sin(angle_z_rad) + y * cos(angle_z_rad);
            x = x_new;
            y = y_new;
        }
    }

    return {x, y, z};
}

Point_4D rotatePoint4D(const Point_4D& point, double angle_xy, double angle_xz, double angle_xw, const std::string& direction) {
    double x = point.x;
    double y = point.y;
    double z = point.z;
    double w = point.t;

    double angle_xy_rad = angle_xy * pi / 180.0;
    double angle_xz_rad = angle_xz * pi / 180.0;
    double angle_xw_rad = angle_xw * pi / 180.0;

    for (char axis : direction) {
        if (axis == 'x' || axis == 'X') {
            double x_new = x * cos(angle_xy_rad) - y * sin(angle_xy_rad);
            double y_new = x * sin(angle_xy_rad) + y * cos(angle_xy_rad);
            x = x_new;
            y = y_new;
        } else if (axis == 'y' || axis == 'Y') {
            double x_new = x * cos(angle_xz_rad) - z * sin(angle_xz_rad);
            double z_new = x * sin(angle_xz_rad) + z * cos(angle_xz_rad);
            x = x_new;
            z = z_new;
        } else if (axis == 'z' || axis == 'Z') {
            double x_new = x * cos(angle_xw_rad) - w * sin(angle_xw_rad);
            double w_new = x * sin(angle_xw_rad) + w * cos(angle_xw_rad);
            x = x_new;
            w = w_new;
        } else if (axis == 't' || axis == 'T') {
            double z_new = z * cos(angle_xz_rad) - w * sin(angle_xz_rad);
            double w_new = z * sin(angle_xz_rad) + w * cos(angle_xz_rad);
            z = z_new;
            w = w_new;
        }
    }

    return {x, y, z, w};
}

Point translatePoint2D(const Point& point, double dx, double dy) {
    return {point.x + dx, point.y + dy};
}

Point_3D translatePoint3D(const Point_3D& point, double dx, double dy, double dz) {
    return {point.x + dx, point.y + dy, point.z + dz};
}

Point_4D translatePoint4D(const Point_4D& point, double dx, double dy, double dz, double dw) {
    return {point.x + dx, point.y + dy, point.z + dz, point.t + dw};
}

Point scalePoint2D(const Point& point, double scaleX, double scaleY) {
    return {point.x * scaleX, point.y * scaleY};
}

Point_3D scalePoint3D(const Point_3D& point, double scaleX, double scaleY, double scaleZ) {
    return {point.x * scaleX, point.y * scaleY, point.z * scaleZ};
}

Point_4D scalePoint4D(const Point_4D& point, double scaleX, double scaleY, double scaleZ, double scaleW) {
    return {point.x * scaleX, point.y * scaleY, point.z * scaleZ, point.t * scaleW};
}

//Matrices
vector<vector<double>> multiplyScalar(vector<vector<double>>& matrix, double scalar) {
    int rows = matrix.size();
    int cols = matrix[0].size();

    // Create a result matrix with the same dimensions as the input matrix
    vector<vector<double>> result(rows, vector<double>(cols));

    // Multiply each element of the matrix by the scalar
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[i][j] = matrix[i][j] * scalar;
        }
    }

    return result;
}

void swapRows(vector<vector<double>>& matrix, int row1, int row2) {
    for (int j = 0; j < matrix[row1].size(); ++j) {
        double temp = matrix[row1][j];
        matrix[row1][j] = matrix[row2][j];
        matrix[row2][j] = temp;
    }
}

// Function to perform row operations to achieve row echelon form
void solveMatrix(vector<vector<double>>& matrix) {
    int lead = 0; // Column number
    int rowCount = matrix.size();
    int columnCount = matrix[0].size();

    for (int r = 0; r < rowCount; ++r) {
        if (columnCount <= lead)
            return;

        int i = r;
        while (matrix[i][lead] == 0) {
            ++i;
            if (rowCount == i) {
                i = r;
                ++lead;
                if (columnCount == lead)
                    return;
            }
        }

        swapRows(matrix, i, r);

        if (matrix[r][lead] != 0) {
            double div = matrix[r][lead];
            for (int j = 0; j < columnCount; ++j)
                matrix[r][j] /= div;
        }

        for (int i = 0; i < rowCount; ++i) {
            if (i != r) {
                double sub = matrix[i][lead];
                for (int j = 0; j < columnCount; ++j)
                    matrix[i][j] -= (sub * matrix[r][j]);
            }
        }

        ++lead;
    }
}

vector<vector<double>> createMatrix() {
    // Get matrix dimensions from the user
    cout << "Enter number of rows for the matrix: ";
    cin >> row;
    cout << "Enter number of columns for the matrix: ";
    cin >> column;

    // Create a vector of vectors to represent the matrix
    vector<vector<double>> matrix(row, vector<double>(column));

    // Take user input for each element of the matrix
    cout << "Enter elements for the matrix:\n";
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < column; ++j) {
            cout << "Enter value for element [" << i << "][" << j << "]: ";
            cin >> matrix[i][j];
        }
    }

    return matrix;
}

int countDecimals(float num) {
    stringstream ss;
    ss << num;
    string numStr = ss.str();

    size_t decimalPos = numStr.find('.');

    if (decimalPos == string::npos) {
        return 0;
    }

    return numStr.length() - decimalPos - 1;
}

void printMatrix(vector<vector<double>> matrix) {
    cout << "\nMatrix:\n";
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < matrix[i].size(); ++j) {
            double value = matrix[i][j];
            int decimals = 0;

            if (countDecimals(value) >= maxDecimals) { decimals = maxDecimals; } else 
            if (countDecimals(value) >= 0 && countDecimals(value) < maxDecimals) { decimals = countDecimals(value); }

            cout << fixed << setprecision(decimals) << value << " ";
        }

        cout << "\n";
    }
}

vector<vector<double>> addMatrices(vector<vector<double>> matrix1, vector<vector<double>> matrix2) {
    // Check if matrices have the same dimensions
    if (matrix1.size() != matrix2.size() || matrix1[0].size() != matrix2[0].size()) {
        cout << "Matrices must have the same dimensions for addition." << endl;
        return {}; // Return an empty matrix
    }

    int rows = matrix1.size();
    int cols = matrix1[0].size();

    // Create a result matrix with the same dimensions as input matrices
    vector<vector<double>> result(rows, vector<double>(cols));

    // Perform addition
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }

    return result;
}

vector<vector<double>> subtractMatrices(vector<vector<double>> matrix1, vector<vector<double>> matrix2) {
    // Check if matrices have the same dimensions
    if (matrix1.size() != matrix2.size() || matrix1[0].size() != matrix2[0].size()) {
        cout << "Matrices must have the same dimensions for subtraction." << endl;
        return {}; // Return an empty matrix
    }

    int rows = matrix1.size();
    int cols = matrix1[0].size();

    // Create a result matrix with the same dimensions as input matrices
    vector<vector<double>> result(rows, vector<double>(cols));

    // Perform addition
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[i][j] = matrix1[i][j] - matrix2[i][j];
        }
    }

    return result;
}

vector<vector<double>> multiplyMatrices(vector<vector<double>> matrix1, vector<vector<double>> matrix2) {
    int rows1 = matrix1.size();
    int cols1 = matrix1[0].size();
    int rows2 = matrix2.size();
    int cols2 = matrix2[0].size();

    // Check if the matrices can be multiplied
    if (cols1 != rows2) {
        cout << "Matrices cannot be multiplied. Number of columns in the first matrix must equal the number of rows in the second matrix." << endl;
        return {}; // Return an empty matrix
    }

    // Create a result matrix with dimensions rows1 x cols2
    vector<vector<double>> result(rows1, vector<double>(cols2, 0));

    // Perform matrix multiplication
    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < cols2; ++j) {
            for (int k = 0; k < cols1; ++k) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}

vector<vector<double>> transpose(vector<vector<double>> matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();

    // Create a matrix with dimensions cols x rows
    vector<vector<double>> transposed(cols, vector<double>(rows));

    // Fill the transposed matrix
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }

    return transposed;
}

double trace(vector<vector<double>> matrix) {
    int trace = 0;
    for (int i = 0; i < matrix.size(); ++i) {
        trace += matrix[i][i]; // Sum of diagonal elements
    }
    return trace;
}

vector<vector<double>> getSubMatrix(vector<vector<double>>& matrix, int row, int col) {
    vector<vector<double>> submatrix;
    int size = matrix.size();

    for (int i = 1; i < size; ++i) {
        vector<double> temp;
        for (int j = 0; j < size; ++j) {
            if (j != col) {
                temp.push_back(matrix[i][j]);
            }
        }
        if (!temp.empty()) {
            submatrix.push_back(temp);
        }
    }
    return submatrix;
}

double determinant(vector<vector<double>>& matrix) {
    int size = matrix.size();
    double det = 0;

    // Base case: If the matrix is 1x1, return the single element
    if (size == 1) {
        return matrix[0][0];
    }
    else {
        for (int col = 0; col < size; ++col) {
            // Get the submatrix by removing the first row and current column
            vector<vector<double>> submatrix = getSubMatrix(matrix, 0, col);
            // Add the determinant of the submatrix multiplied by the corresponding element of the first row
            det += (col % 2 == 0 ? 1 : -1) * matrix[0][col] * determinant(submatrix);
        }
    }
    return det;
}

vector<vector<double>> inverse(vector<vector<double>> matrix) {
    int n = matrix.size();
    // Augmenting the square matrix with the identity matrix of the same size
    vector<vector<double>> augmentedMatrix(n, vector<double>(2 * n));

    // Filling the augmented matrix
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            augmentedMatrix[i][j] = matrix[i][j];
        }
        augmentedMatrix[i][i + n] = 1;
    }

    // Applying Gaussian elimination
    for (int i = 0; i < n; ++i) {
        // Swapping rows if necessary to make the diagonal element non-zero
        if (augmentedMatrix[i][i] == 0) {
            int j = i + 1;
            while (j < n && augmentedMatrix[j][i] == 0) ++j;
            if (j == n) {
                cout << "Matrix is singular. Inverse does not exist." << endl;
                return {};
            }
            swap(augmentedMatrix[i], augmentedMatrix[j]);
        }

        // Making all elements of the column except the diagonal element zero
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                double ratio = augmentedMatrix[j][i] / augmentedMatrix[i][i];
                for (int k = 0; k < 2 * n; ++k) {
                    augmentedMatrix[j][k] -= ratio * augmentedMatrix[i][k];
                }
            }
        }
    }

    // Scaling rows to make diagonal elements 1
    for (int i = 0; i < n; ++i) {
        double div = augmentedMatrix[i][i];
        for (int j = 0; j < 2 * n; ++j) {
            augmentedMatrix[i][j] /= div;
        }
    }

    // Extracting the inverse matrix from the augmented matrix
    vector<vector<double>> inverseMatrix(n, vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            inverseMatrix[i][j] = augmentedMatrix[i][j + n];
        }
    }

    return inverseMatrix;
}

vector<vector<double>> rotateMatrix2D(const vector<vector<double>>& matrix, double angle) {
    double angle_rad = angle * pi / 180.0;
    double cos_angle = cos(angle_rad);
    double sin_angle = sin(angle_rad);

    // Create a new matrix to hold the rotated points
    vector<vector<double>> rotated(matrix.size(), vector<double>(2));

    // Iterate through each point in the matrix and perform rotation
    for (int i = 0; i < matrix.size(); ++i) {
        double x = matrix[i][0];
        double y = matrix[i][1];

        double x_rotated = x * cos_angle - y * sin_angle;
        double y_rotated = x * sin_angle + y * cos_angle;

        rotated[i][0] = x_rotated;
        rotated[i][1] = y_rotated;
    }

    return rotated;
}

vector<vector<double>> rotateMatrix3D(const vector<vector<double>>& matrix, double angle_x, double angle_y, double angle_z, const std::string& direction) {
    double angle_x_rad = angle_x * pi / 180.0;
    double cos_x = cos(angle_x_rad);
    double sin_x = sin(angle_x_rad);

    double angle_y_rad = angle_y * pi / 180.0;
    double cos_y = cos(angle_y_rad);
    double sin_y = sin(angle_y_rad);

    double angle_z_rad = angle_z * pi / 180.0;
    double cos_z = cos(angle_z_rad);
    double sin_z = sin(angle_z_rad);

    vector<vector<double>> rotated(matrix.size(), vector<double>(3));

    for (int i = 0; i < matrix.size(); ++i) {
        double x = matrix[i][0];
        double y = matrix[i][1];
        double z = matrix[i][2];

        for (char axis : direction) {
            if (axis == 'x' || axis == 'X') {
                double y_rotated = y * cos_x - z * sin_x;
                double z_rotated = y * sin_x + z * cos_x;
                y = y_rotated;
                z = z_rotated;
            } else if (axis == 'y' || axis == 'Y') {
                double x_rotated = x * cos_y + z * sin_y;
                double z_rotated = -x * sin_y + z * cos_y;
                x = x_rotated;
                z = z_rotated;
            } else if (axis == 'z' || axis == 'Z') {
                double x_rotated = x * cos_z - y * sin_z;
                double y_rotated = x * sin_z + y * cos_z;
                x = x_rotated;
                y = y_rotated;
            }
        }

        rotated[i][0] = x;
        rotated[i][1] = y;
        rotated[i][2] = z;
    }

    return rotated;
}

vector<vector<double>> rotateMatrix4D(const vector<vector<double>>& matrix, double angle_xy, double angle_xz, double angle_xw, const std::string& direction) {
    double angle_xy_rad = angle_xy * pi / 180.0;
    double cos_xy = cos(angle_xy_rad);
    double sin_xy = sin(angle_xy_rad);

    double angle_xz_rad = angle_xz * pi / 180.0;
    double cos_xz = cos(angle_xz_rad);
    double sin_xz = sin(angle_xz_rad);

    double angle_xw_rad = angle_xw * pi / 180.0;
    double cos_xw = cos(angle_xw_rad);
    double sin_xw = sin(angle_xw_rad);

    vector<vector<double>> rotated(matrix.size(), vector<double>(4));

    for (int i = 0; i < matrix.size(); ++i) {
        double x = matrix[i][0];
        double y = matrix[i][1];
        double z = matrix[i][2];
        double w = matrix[i][3];

        for (char axis : direction) {
            if (axis == 'x' || axis == 'X') {
                double x_rotated = x * cos_xy - y * sin_xy;
                double y_rotated = x * sin_xy + y * cos_xy;
                x = x_rotated;
                y = y_rotated;
            } else if (axis == 'y' || axis == 'Y') {
                double x_rotated = x * cos_xz - z * sin_xz;
                double z_rotated = x * sin_xz + z * cos_xz;
                x = x_rotated;
                z = z_rotated;
            } else if (axis == 'z' || axis == 'Z') {
                double x_rotated = x * cos_xw - w * sin_xw;
                double w_rotated = x * sin_xw + w * cos_xw;
                x = x_rotated;
                w = w_rotated;
            } else if (axis == 't' || axis == 'T') {
                double z_rotated = z * cos_xz - w * sin_xz;
                double w_rotated = z * sin_xz + w * cos_xz;
                z = z_rotated;
                w = w_rotated;
            }
        }

        rotated[i][0] = x;
        rotated[i][1] = y;
        rotated[i][2] = z;
        rotated[i][3] = w;
    }

    return rotated;
}

vector<vector<double>> translateMatrix2D(const vector<vector<double>>& matrix, double dx, double dy) {
    // Create a new matrix to hold the translated points
    vector<vector<double>> translated(matrix.size(), vector<double>(2));

    // Iterate through each point in the matrix and perform translation
    for (int i = 0; i < matrix.size(); ++i) {
        translated[i][0] = matrix[i][0] + dx;
        translated[i][1] = matrix[i][1] + dy;
    }

    return translated;
}

vector<vector<double>> translateMatrix3D(const vector<vector<double>>& matrix, double dx, double dy, double dz) {
    // Create a new matrix to hold the translated points
    vector<vector<double>> translated(matrix.size(), vector<double>(3));

    // Iterate through each point in the matrix and perform translation
    for (int i = 0; i < matrix.size(); ++i) {
        translated[i][0] = matrix[i][0] + dx;
        translated[i][1] = matrix[i][1] + dy;
        translated[i][2] = matrix[i][2] + dz;
    }

    return translated;
}

vector<vector<double>> translateMatrix4D(const vector<vector<double>>& matrix, double dx, double dy, double dz, double dw) {
    // Create a new matrix to hold the translated points
    vector<vector<double>> translated(matrix.size(), vector<double>(4));

    // Iterate through each point in the matrix and perform translation
    for (int i = 0; i < matrix.size(); ++i) {
        translated[i][0] = matrix[i][0] + dx;
        translated[i][1] = matrix[i][1] + dy;
        translated[i][2] = matrix[i][2] + dz;
        translated[i][3] = matrix[i][3] + dw;
    }

    return translated;
}

vector<vector<double>> scaleMatrix2D(const vector<vector<double>>& matrix, double scaleX, double scaleY) {
    // Create a new matrix to hold the scaled points
    vector<vector<double>> scaled(matrix.size(), vector<double>(2));

    // Iterate through each point in the matrix and perform scaling
    for (int i = 0; i < matrix.size(); ++i) {
        scaled[i][0] = matrix[i][0] * scaleX;
        scaled[i][1] = matrix[i][1] * scaleY;
    }

    return scaled;
}

vector<vector<double>> scaleMatrix3D(const vector<vector<double>>& matrix, double scaleX, double scaleY, double scaleZ) {
    // Create a new matrix to hold the scaled points
    vector<vector<double>> scaled(matrix.size(), vector<double>(3));

    // Iterate through each point in the matrix and perform scaling
    for (int i = 0; i < matrix.size(); ++i) {
        scaled[i][0] = matrix[i][0] * scaleX;
        scaled[i][1] = matrix[i][1] * scaleY;
        scaled[i][2] = matrix[i][2] * scaleZ;
    }

    return scaled;
}

vector<vector<double>> scaleMatrix4D(const vector<vector<double>>& matrix, double scaleX, double scaleY, double scaleZ, double scaleW) {
    // Create a new matrix to hold the scaled points
    vector<vector<double>> scaled(matrix.size(), vector<double>(4));

    // Iterate through each point in the matrix and perform scaling
    for (int i = 0; i < matrix.size(); ++i) {
        scaled[i][0] = matrix[i][0] * scaleX;
        scaled[i][1] = matrix[i][1] * scaleY;
        scaled[i][2] = matrix[i][2] * scaleZ;
        scaled[i][3] = matrix[i][3] * scaleW;
    }

    return scaled;
}

//Conic sections
Point circle(double x, double y, double horizontalShift, double verticalShift, double radius, char coordinateType) {
    Point point;

    if (coordinateType == 'x' || coordinateType == 'X') {
        point.x = sqrt(pow(radius, 2) - pow(y - verticalShift, 2)) + horizontalShift;
        point.y = y;
    } else if (coordinateType == 'y' || coordinateType == 'Y') {
        point.x = x;
        point.y = sqrt(pow(radius, 2) - pow(x - horizontalShift, 2)) + verticalShift;
    } else {
        point.x = NAN;
        point.y = NAN;
    }

    return point;
}

Point_3D sphere(double x, double y, double z, double horizontalShift, double verticalShift, double zShift, double radius, char coordinateType) {
    Point_3D point;

    if (coordinateType == 'x' || coordinateType == 'X') {
        point.x = sqrt(pow(radius, 2) - pow(y - verticalShift, 2) - pow(z - zShift, 2)) + horizontalShift;
        point.y = y;
        point.z = z;
    } else if (coordinateType == 'y' || coordinateType == 'Y') {
        point.x = x;
        point.y = sqrt(pow(radius, 2) - pow(x - horizontalShift, 2) - pow(z - zShift, 2)) + verticalShift;
        point.z = z;
    } else if (coordinateType == 'z' || coordinateType == 'Z') {
        point.x = x;
        point.y = y;
        point.z = sqrt(pow(radius, 2) - pow(y - verticalShift, 2) - pow(x - horizontalShift, 2)) + zShift;
    } else {
        point.x = NAN;
        point.y = NAN;
        point.z = NAN;
    }

    return point;
}

Point_4D hyperSphere(double x, double y, double z, double t, double horizontalShift, double verticalShift, double zShift, double tShift, double radius, char coordinateType) {
    Point_4D point;

    if (coordinateType == 'x' || coordinateType == 'X') {
        point.x = sqrt(pow(radius, 2) - pow(y - verticalShift, 2) - pow(z - zShift, 2) - pow(t - tShift, 2)) + horizontalShift;
        point.y = y;
        point.z = z;
        point.t = t;
    } else if (coordinateType == 'y' || coordinateType == 'Y') {
        point.x = x;
        point.y = sqrt(pow(radius, 2) - pow(x - horizontalShift, 2) - pow(z - zShift, 2) - pow(t - tShift, 2)) + verticalShift;
        point.z = z;
        point.t = t;
    } else if (coordinateType == 'z' || coordinateType == 'Z') {
        point.x = x;
        point.y = y;
        point.z = sqrt(pow(radius, 2) - pow(y - verticalShift, 2) - pow(x - horizontalShift, 2) - pow(t - tShift, 2)) + zShift;
        point.t = t;
    } else if (coordinateType == 't' || coordinateType == 'T') {
        point.x = x;
        point.y = y;
        point.z = z;
        point.t = sqrt(pow(radius, 2) - pow(y - verticalShift, 2) - pow(x - horizontalShift, 2) - pow(z - zShift, 2)) + tShift;
    } else {
        point.x = NAN;
        point.y = NAN;
        point.z = NAN;
    }

    return point;
}

Point circleAngle(double angle, double radius, double horizontalShift, double verticalShift) {
    angle *= pi / 180;
    Point point;
    point.x = radius * cos(angle) + horizontalShift;
    point.y = radius * sin(angle) + verticalShift;
    return point;
}

Point_3D sphereAngle(double xAngle, double yAngle, double zAngle, double radius, double horizontalShift, double verticalShift, double zShift) {
    xAngle *= pi / 180;
    yAngle *= pi / 180;
    zAngle *= pi / 180;
    
    Point_3D point;
    point.x = radius * cos(xAngle) + horizontalShift;
    point.y = radius * sin(yAngle) + verticalShift; 
    point.z = radius * cos(zAngle) + zShift; 
    
    return point;
}

Point_4D hyperSphereAngle(double xAngle, double yAngle, double zAngle, double wAngle, double radius, double horizontalShift, double verticalShift, double zShift, double wShift) {
    xAngle *= pi / 180;
    yAngle *= pi / 180;
    zAngle *= pi / 180;
    wAngle *= pi / 180;
    
    Point_4D point;
    point.x = radius * cos(xAngle) + horizontalShift;
    point.y = radius * cos(yAngle) + verticalShift;
    point.z = radius * cos(zAngle) + zShift;
    point.t = radius * cos(wAngle) + wShift;

    return point;
}

Point ellipse(double x, double y, double a, double b, double horizontalShift, double verticalShift, char coordinateType) {
    Point point;

    if (coordinateType == 'x' || coordinateType == 'X') {
        point.x = sqrt((1 - pow((y - verticalShift) / b, 2)) * pow(a, 2)) + horizontalShift;
        point.y = y;
    } else if (coordinateType == 'y' || coordinateType == 'Y') {
        point.x = x;
        point.y = sqrt((1 - pow(x - horizontalShift / a, 2)) * pow(b, 2)) + verticalShift;
    }

    return point;
}

Point_3D ellipsoid_3D(double x, double y, double z, double a, double b, double c, double horizontalShift, double verticalShift, double zShift, char coordinateType) {
    Point_3D point;

    if (coordinateType == 'x' || coordinateType == 'X') {
        point.x = sqrt((1 - pow((y - verticalShift) / b, 2) - pow((z - zShift) / c, 2)) * pow(a, 2)) + horizontalShift;
        point.y = y;
        point.z = z;
    } else if (coordinateType == 'y' || coordinateType == 'Y') {
        point.x = x;
        point.y = sqrt((1 - pow((x - horizontalShift) / a, 2) - pow((z - zShift) / c, 2)) * pow(b, 2)) + verticalShift;
        point.z = z;
    } else if (coordinateType == 'z' || coordinateType == 'Z') {
        point.x = x;
        point.y = y;
        point.z = sqrt((1 - pow((x - horizontalShift) / a, 2) - pow((y - verticalShift) / b, 2)) * pow(c, 2)) + zShift;
    }

    return point;
}

Point_4D ellipsoid_4D(double x, double y, double z, double t, double a, double b, double c, double d, double horizontalShift, double verticalShift, double zShift, double tShift, char coordinateType) {
    Point_4D point;

    if (coordinateType == 'x' || coordinateType == 'X') {
        point.x = sqrt((1 - pow((y - verticalShift) / b, 2) - pow((z - zShift) / c, 2) - pow((t - tShift) / d, 2)) * pow(a, 2)) + horizontalShift;
        point.y = y;
        point.z = z;
        point.t = t;
    } else if (coordinateType == 'y' || coordinateType == 'Y') {
        point.x = x;
        point.y = sqrt((1 - pow((x - horizontalShift) / a, 2) - pow((z - zShift) / c, 2) - pow((t - tShift) / d, 2)) * pow(b, 2)) + verticalShift;
        point.z = z;
        point.t = t;
    } else if (coordinateType == 'z' || coordinateType == 'Z') {
        point.x = x;
        point.y = y;
        point.z = sqrt((1 - pow((x - horizontalShift) / a, 2) - pow((y - verticalShift) / b, 2) - pow((t - tShift) / d, 2)) * pow(c, 2)) + zShift;
        point.t = t;
    } else if (coordinateType == 't' || coordinateType == 'T') {
        point.x = x;
        point.y = y;
        point.z = z;
        point.t = sqrt((1 - pow((x - horizontalShift) / a, 2) - pow((y - verticalShift) / b, 2) - pow((z - zShift) / c, 2)) * pow(d, 2)) + tShift;
    }

    return point;
}

Point ellipseAngle(double angle, double a, double b, double horizontalShift, double verticalShift) {
    angle *= pi / 180;
    Point point;
    point.x = a * cos(angle) + horizontalShift;
    point.y = b * sin(angle) + verticalShift;
    return point;
}

Point_3D ellipsoid_3DAngle(double xAngle, double yAngle, double zAngle, double a, double b, double c, double horizontalShift, double verticalShift, double zShift) {
    xAngle *= pi / 180;
    yAngle *= pi / 180;
    zAngle *= pi / 180;
    Point_3D point;
    point.x = a * cos(xAngle) + horizontalShift;
    point.y = b * sin(yAngle) + verticalShift;
    point.z = c * cos(zAngle) + zShift;
    return point;
}

Point_4D ellipsoid_4DAngle(double xAngle, double yAngle, double zAngle, double wAngle, double a, double b, double c, double d, double horizontalShift, double verticalShift, double zShift, double wShift) {
    xAngle *= pi / 180;
    yAngle *= pi / 180;
    zAngle *= pi / 180;
    wAngle *= pi / 180;
    Point_4D point;
    point.x = a * cos(xAngle) + horizontalShift;
    point.y = b * cos(yAngle) + verticalShift;
    point.z = c * cos(zAngle) + zShift;
    point.t = d * cos(wAngle) + wShift;
    return point;
}

Point xParabola(double x, double a, double verticalShift, double horizontalShift) { return {x, a * pow(x - horizontalShift, 2) + verticalShift}; }

Point_3D parabola_3D(double x, double y, double a, double b, double c, double d, double f, double verticalShift, double horizontalShift, double zShift) {
    double z = a * pow(x - horizontalShift, 2) + b * pow(y + verticalShift, 2) + c * x + d * y + f + zShift;
    return {x, y, z};
}

Point_4D parabola_4D(double x, double y, double z, double a, double b, double c, double d, double f, double g, double h, double verticalShift, double horizontalShift, double zShift, double tShift) {
    double t = a * pow(x - horizontalShift, 2) + b * pow(y + verticalShift, 2) + c * pow(z - zShift, 2) + d * x + f * y + g * z + h + tShift;
    return {x, y, z, t};
}

Point yParabola(double y, double a, double verticalShift, double horizontalShift) { return {a * pow(y - verticalShift, 2) + horizontalShift, y}; }

Point horizontalHyperbola(double y, double horizontalShift, double verticalShift, double a, double b) { return {horizontalShift + a * sqrt(1 + pow(y - verticalShift, 2) / pow(b, 2)), y}; }

Point verticalHyperbola(double x, double horizontalShift, double verticalShift, double a, double b) { return {x, verticalShift + b * sqrt(1 + pow(x - horizontalShift, 2) / pow(a, 2))}; }

Point_3D hyperbola_3D(double x, double y, double a, double b, double c, double d, double f, double verticalShift, double horizontalShift, double zShift) {
    double z = c * sqrt(1 + pow((x - horizontalShift) / c, 2)) + d * sqrt(1 + pow((y + verticalShift) / d, 2)) + f + zShift;
    return {x, y, z};
}

Point_4D hyperbola_4D(double x, double y, double z, double a, double b, double c, double d, double f, double g, double h, double verticalShift, double horizontalShift, double zShift, double tShift) {
    double t = a * sqrt(1 + pow((x - horizontalShift) / a, 2)) + b * sqrt(1 + pow((y + verticalShift) / b, 2)) + c * sqrt(1 + pow((z - zShift) / c, 2)) + d * x + f * y + g * z + h + tShift;
    return {x, y, z, t};
}

//Rotate points
Point rotatePoint(Point point, double theta) {
    theta *= pi / 180;
    double cosTheta = cos(theta);
    double sinTheta = sin(theta);
    double newX = point.x * cosTheta - point.y * sinTheta;
    double newY = point.x * sinTheta + point.y * cosTheta;
    point.x = newX;
    point.y = newY;
    return {newX, newY};
}

Point_3D rotatePoint3D(Point_3D point, double thetaX, double thetaY, double thetaZ, string& order) {
    double cosX = cos(thetaX * pi / 180);
    double sinX = sin(thetaX * pi / 180);
    double cosY = cos(thetaY * pi / 180);
    double sinY = sin(thetaY * pi / 180);
    double cosZ = cos(thetaZ * pi / 180);
    double sinZ = sin(thetaZ * pi / 180);

    double newX = point.x;
    double newY = point.y * cosX - point.z * sinX;
    double newZ = point.y * sinX + point.z * cosX;

    for (char c : order) {
        switch (c) {
            case 'X':
            case 'x':
                {
                    double tempY = newY;
                    newY = tempY * cosX - newZ * sinX;
                    newZ = tempY * sinX + newZ * cosX;
                }
                break;
            case 'Y':
            case 'y':
                {
                    double tempX = newX;
                    newX = tempX * cosY + newZ * sinY;
                    newZ = -tempX * sinY + newZ * cosY;
                }
                break;
            case 'Z':
            case 'z':
                {
                    double tempX = newX;
                    newX = tempX * cosZ - newY * sinZ;
                    newY = tempX * sinZ + newY * cosZ;
                }
                break;
            default:
                // Invalid rotation order
                throw invalid_argument("Invalid rotation order");
        }
    }

    return {newX, newY, newZ};
}

Point_4D rotatePoint4D(Point_4D point, double thetaX, double thetaY, double thetaZ, double thetaT, string& order) {
    double cosX = cos(thetaX * pi / 180 / 2);
    double sinX = sin(thetaX * pi / 180 / 2);
    double cosY = cos(thetaY * pi / 180 / 2);
    double sinY = sin(thetaY * pi / 180 / 2);
    double cosZ = cos(thetaZ * pi / 180 / 2);
    double sinZ = sin(thetaZ * pi / 180 / 2);
    double cosT = cos(thetaT * pi / 180 / 2);
    double sinT = sin(thetaT * pi / 180 / 2);

    double qx = sinX, qy = sinY, qz = sinZ, qw = sinT;

    double newX = point.x, newY = point.y, newZ = point.z, newW = point.t;

    for (char c : order) {
        switch (c) {
            case 'x':
                {
                    double tempY = newY;
                    newY = tempY * cosX - newZ * sinX;
                    newZ = tempY * sinX + newZ * cosX;
                }
                break;
            case 'y':
                {
                    double tempX = newX;
                    newX = tempX * cosY + newZ * sinY;
                    newZ = -tempX * sinY + newZ * cosY;
                }
                break;
            case 'z':
                {
                    double tempX = newX;
                    newX = tempX * cosZ - newY * sinZ;
                    newY = tempX * sinZ + newY * cosZ;
                }
                break;
            case 't':
                {
                    double tempX = newX;
                    newX = tempX * cosT - newW * sinT;
                    newW = tempX * sinT + newW * cosT;
                }
                break;
            default:
                // Invalid rotation order
                throw std::invalid_argument("Invalid rotation order");
        }
    }

    return {newX, newY, newZ, newW};
}

//Perimeter
double circlePerimeter(double radius) { return 2 * pi * radius; }

double ellipseArgument(double theta, double a, double b) { 
    if (b > a) {
        double temp = a;
        a = b;
        b = temp;
    }
    
    double e = sqrt(pow(a, 2) - pow(b, 2)) / a;
    return sqrt(1 - pow(e * sin(theta), 2)); 
}

double ellipsePerimeter(double a, double b) {
    if (b > a) {
        double temp = a;
        a = b;
        b = temp;
    }

    int numSteps = 1e8;
    double step_size = (pi / 2) / numSteps;
    double integral = 0;

    for (int i = 0; i < numSteps; i++) {
        double x1 = i * step_size;
        double x2 = (i + 1) * step_size;
        integral += (ellipseArgument(x1, a, b) + ellipseArgument(x2, a, b)) * step_size / 2;
    }

    double result = 4 * a * integral;
    cout << fixed << setprecision(12);
    return result;
}

double approximateEllipsePerimeter(double a, double b) {
    if (b > a) {
        double temp = a;
        a = b;
        b = temp;
    }

    double h = pow((a - b) / (a + b), 2);
    double result =  pi * (a + b) * (1 + 3 * h / (10 + pow(4 - 3 * h, 0.5)));
    cout << fixed << setprecision(12);
    return result;
}

double binomialCoefficient(int n) {
    double result = 1.0;
    for (int i = 1; i <= n; ++i) {
        result *= (double)(2 * n - i + 1) / i;
    }
    return result;
}

double summationEllipsePerimeter(double a, double b) {
    if (b > a) {
        double temp = a;
        a = b;
        b = temp;
    }

    const int precision = 1000;
    
    double perimeter = pi * (a + b);
    double h = ((a - b) * (a - b)) / ((a + b) * (a + b));
    double sumPart = 0;

    for (int n = 1; n <= precision; ++n) {
        double term = pow(1.0 / 2, 2 * n) * pow(h, n) / (n * binomialCoefficient(n));
        sumPart += term;
        if (term < 1.0e-15) // Stop if term is small
            break;
    }

    perimeter *= (1 + 2 * sumPart);
    cout << fixed << setprecision(12);
    return perimeter;
}    

//Area
double circleArea(double radius) { return pi * pow(radius, 2); }
double ellipseArea(double a, double b) { return pi * a * b; }

//Section length
double circleArcLength(double angle, double radius) { return radius * (angle * pi / 180); }

double ellipseArcLength(double theta, double a, double b) {
    if (b > a) {
        double temp = a;
        a = b;
        b = temp;
    }

    theta *= pi / 180;
    int n = 10000; // Number of intervals for numerical integration
    double h = theta / n;
    double sum = 0.5 * (sqrt(pow(b, 2)) + sqrt(pow(a * sin(theta), 2) + pow(b * cos(theta), 2)));
    
    for (int i = 1; i < n; i++) {
        double angle = i * h;
        sum += sqrt(pow(a * sin(angle), 2) + pow(b * cos(angle), 2));
    }

    cout << fixed << setprecision(12);
    return sum * h;
}

//Sectional area
double circleSectionalArea(double angle, double radius) { return pow(radius * (angle * pi / 180) / 2, 2); }

double ellipseSectionalArea(double angle, double a, double b) { return (angle * pi * a * b / 360); }

//Other
double ellipseEccentricity(double a, double b) { 
    if (a > b) {
        return pow(pow(a, 2) - pow(b, 2), 0.5) / a; 
    } else if (a < b) {
        return pow(pow(b, 2) - pow(a, 2), 0.5) / b; 
    } else { return 0; }
}

/*****************
-----Graphing-----
******************/
void drawRect(int x, int y, int width, int height, int borderWidth, string borderHex, string fillColorHex) {
    // Open an SVG file for writing
    ofstream svgFile("rectangle.svg");

    // SVG header
    svgFile << "<svg width=\"" << width + x << "\" height=\"" << height + y << "\" xmlns=\"http://www.w3.org/2000/svg\">" << endl;

    // Border rectangle
    svgFile << "<rect x=\"" << x + borderWidth / 2 << "\" y=\"" << y + borderWidth / 2 << "\" width=\"" 
            << width - borderWidth << "\" height=\"" << height - borderWidth << "\" ";

    // Border style and fill color
    svgFile << "style=\"fill:#" << fillColorHex << ";stroke:#" << borderHex << ";stroke-width:" << borderWidth << "\"/>" << endl;

    // Close SVG tag
    svgFile << "</svg>";

    // Close the file
    svgFile.close();

    cout << "Rectangle created in rectangle.svg" << endl;
}

void createEmptyRect(int x, int y, int width, int height, int borderWidth, string borderHex) {
    // Open an SVG file for writing
    ofstream svgFile("empty_rectangle.svg");

    // SVG header
    svgFile << "<svg width=\"" << width + x << "\" height=\"" << height + y << "\" xmlns=\"http://www.w3.org/2000/svg\">" << endl;

    // Border rectangle
    svgFile << "<rect x=\"" << x + borderWidth / 2 << "\" y=\"" << y + borderWidth / 2 << "\" width=\"" 
            << width - borderWidth << "\" height=\"" << height - borderWidth << "\" ";

    // Border style
    svgFile << "style=\"fill:none;stroke:#" << borderHex << ";stroke-width:" << borderWidth << "\"/>" << endl;

    // Close SVG tag
    svgFile << "</svg>";

    // Close the file
    svgFile.close();

    cout << "Empty rectangle created in empty_rectangle.svg" << endl;
}

void fillRect(int x, int y, int width, int height, string fillColorHex) {
    // Open an SVG file for writing
    ofstream svgFile("filled_rectangle.svg");

    // SVG header
    svgFile << "<svg width=\"" << width + x << "\" height=\"" << height + y << "\" xmlns=\"http://www.w3.org/2000/svg\">" << endl;

    // Filled rectangle
    svgFile << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << width << "\" height=\"" << height << "\" ";
    
    // Fill color
    svgFile << "style=\"fill:#" << fillColorHex << "\"/>" << endl;

    // Close SVG tag
    svgFile << "</svg>";

    // Close the file
    svgFile.close();

    cout << "Filled rectangle created in filled_rectangle.svg" << endl;
}

void drawEllipse(int x, int y, int a, int b, int borderWidth, string borderHex, string fillColorHex) {
    // Open an SVG file for writing
    ofstream svgFile("ellipse.svg");

    // SVG header
    svgFile << "<svg width=\"" << a + borderWidth + x << "\" height=\"" << b + borderWidth + y << "\" xmlns=\"http://www.w3.org/2000/svg\">" << endl;

    // Border ellipse
    svgFile << "<ellipse cx=\"" << x + a / 2 << "\" cy=\"" << y + b / 2 << "\" rx=\"" << a / 2 << "\" ry=\"" << b / 2 << "\" ";

    // Border style and fill color
    svgFile << "style=\"fill:#" << fillColorHex << ";stroke:#" << borderHex << ";stroke-width:" << borderWidth << "\"/>" << endl;

    // Close SVG tag
    svgFile << "</svg>";

    // Close the file
    svgFile.close();

    cout << "Ellipse created in ellipse.svg" << endl;
}

void createEmptyEllipse(int x, int y, int a, int b, int borderWidth, string borderHex) {
    // Open an SVG file for writing
    ofstream svgFile("empty_ellipse.svg");

    // SVG header
    svgFile << "<svg width=\"" << a + borderWidth + x << "\" height=\"" << b + borderWidth + y << "\" xmlns=\"http://www.w3.org/2000/svg\">" << endl;

    // Border ellipse
    svgFile << "<ellipse cx=\"" << x + a / 2 << "\" cy=\"" << y + b / 2 << "\" rx=\"" << a / 2 << "\" ry=\"" << b / 2 << "\" ";

    // Border style
    svgFile << "style=\"fill:none;stroke:#" << borderHex << ";stroke-width:" << borderWidth << "\"/>" << endl;

    // Close SVG tag
    svgFile << "</svg>";

    // Close the file
    svgFile.close();

    cout << "Empty ellipse created in empty_ellipse.svg" << endl;
}

void fillEllipse(int x, int y, int a, int b, string fillColorHex) {
    // Open an SVG file for writing
    ofstream svgFile("filled_ellipse.svg");

    // SVG header
    svgFile << "<svg width=\"" << a + x << "\" height=\"" << b + y << "\" xmlns=\"http://www.w3.org/2000/svg\">" << endl;

    // Filled ellipse
    svgFile << "<ellipse cx=\"" << x + a / 2 << "\" cy=\"" << y + b / 2 << "\" rx=\"" << a / 2 << "\" ry=\"" << b / 2 << "\" ";
    
    // Fill color
    svgFile << "style=\"fill:#" << fillColorHex << "\"/>" << endl;

    // Close SVG tag
    svgFile << "</svg>";

    // Close the file
    svgFile.close();

    cout << "Filled ellipse created in filled_ellipse.svg" << endl;
}

void drawLine(int x1, int y1, int x2, int y2, int width, string colorHex) {
    // Open an SVG file for writing
    ofstream svgFile("line.svg");

    // SVG header
    svgFile << "<svg width=\"" << max(x1, x2) + width << "\" height=\"" << max(y1, y2) + width << "\" xmlns=\"http://www.w3.org/2000/svg\">" << endl;

    // Draw line
    svgFile << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\" ";
    
    // Line style
    svgFile << "style=\"stroke:#" << colorHex << ";stroke-width:" << width << "\"/>" << endl;

    // Close SVG tag
    svgFile << "</svg>";

    // Close the file
    svgFile.close();

    cout << "Line created in line.svg" << endl;
}

void drawPolygon(vector<Point>& points, int borderWidth, string borderHex, string fillColorHex) {
    // Find bounding box of the polygon
    double minX = points[0].x, minY = points[0].y;
    double maxX = points[0].x, maxY = points[0].y;

    for (auto& point : points) {
        minX = min(minX, point.x);
        minY = min(minY, point.y);
        maxX = max(maxX, point.x);
        maxY = max(maxY, point.y);
    }

    double width = maxX - minX;
    double height = maxY - minY;

    // Open an SVG file for writing
    ofstream svgFile("polygon.svg");

    // SVG header
    svgFile << "<svg width=\"" << width + borderWidth << "\" height=\"" << height + borderWidth << "\" xmlns=\"http://www.w3.org/2000/svg\">" << endl;

    // Draw polygon edges
    svgFile << "<polygon points=\"";
    for (auto& point : points) { svgFile << point.x - minX << "," << point.y - minY << " "; }
    svgFile << "\" ";

    // Border style and fill color
    svgFile << "style=\"fill:#" << fillColorHex << ";stroke:#" << borderHex << ";stroke-width:" << borderWidth << "\"/>" << endl;

    // Close SVG tag
    svgFile << "</svg>";

    // Close the file
    svgFile.close();

    cout << "Polygon created in polygon.svg" << endl;
}

double evaluate(Function func, double x) {
    return static_cast<double>(func(x));
}

void plotGraph(Function func, double minX, double maxX, double minY, double maxY, double stepSize, const string& hexLineColor) {
    // Calculate number of points to plot
    int numPoints = static_cast<int>((maxX - minX) / stepSize) + 1;

    // Vector to store points
    vector<Point> points(numPoints);

    // Evaluate function at each step and store points
    for (int i = 0; i < numPoints; ++i) {
        double x = minX + i * stepSize;
        double y = evaluate(func, x);
        // Check if y is NaN, if so, skip this point
        if (!isnan(y)) {
            points[i] = {x, y};
        } else {
            // Set x-coordinate of the skipped point to NaN so it's not drawn
            points[i] = {NAN, NAN};
        }
    }

    // Normalize points to fit in the specified range
    double xRange = maxX - minX;
    double yRange = maxY - minY;

    for (auto& point : points) {
        if (!isnan(point.x)) {
            point.x = (point.x - minX) * SVG_SIZE / xRange;
            point.y = (point.y - minY) * SVG_SIZE / yRange;
        }
    }

    // Calculate the x-position of the y-axis
    double yAxisXPos = -minX * SVG_SIZE / xRange;
    if (minX <= 0 && maxX >= 0) {
        yAxisXPos = -minX / (maxX - minX) * SVG_SIZE;
    } else if (minX > 0) {
        yAxisXPos = 0;
    } else {
        yAxisXPos = SVG_SIZE;
    }

    // Calculate the y-position of the x-axis
    double xAxisYPos;
    if (minY <= 0 && maxY >= 0) {
        xAxisYPos = maxY / (maxY - minY) * SVG_SIZE;
    } else if (minY > 0) {
        xAxisYPos = SVG_SIZE;
    } else {
        xAxisYPos = 0;
    }

    // Open SVG file for writing
    ofstream svgFile("graph.svg");
    // SVG header
    svgFile << "<svg width=\"" << SVG_SIZE << "\" height=\"" << SVG_SIZE << "\" xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;
    // Draw x-axis
    svgFile << "<line x1=\"" << yAxisXPos << "\" y1=\"0\" x2=\"" << yAxisXPos << "\" y2=\"" << SVG_SIZE << "\" style=\"stroke:black;stroke-width:1\"/>" << std::endl;
    // Draw y-axis
    svgFile << "<line x1=\"0\" y1=\"" << xAxisYPos << "\" x2=\"" << SVG_SIZE << "\" y2=\"" << xAxisYPos << "\" style=\"stroke:black;stroke-width:1\"/>" << std::endl;

    // Plot points and lines
    for (int i = 1; i < numPoints; ++i) {
        // Check if both x-coordinates are not NaN
        if (!isnan(points[i - 1].x) && !isnan(points[i].x)) {
            // Check if both y-coordinates are not NaN
            if (!isnan(points[i - 1].y) && !isnan(points[i].y)) {
                // Draw line between two points
                svgFile << "<line x1=\"" << points[i - 1].x << "\" y1=\"" << SVG_SIZE - points[i - 1].y << "\" "
                        << "x2=\"" << points[i].x << "\" y2=\"" << SVG_SIZE - points[i].y << "\" "
                        << "style=\"stroke:" << hexLineColor << ";stroke-width:2\"/>" << std::endl;
            }
        }
    }

    // Close SVG tag
    svgFile << "</svg>";
    // Close the file
    svgFile.close();
    cout << "Graph created in graph.svg" << endl;
    
    //Open HTML file for writing
    ofstream htmlFile("graph.html");
    // HTML header
    htmlFile << "<!DOCTYPE html>" << endl;
    htmlFile << "<html>" << endl;
    htmlFile << "   <head>" << endl;
    htmlFile << "       <title>Graph</title>" << endl;
    htmlFile << "       <script>" << endl;
    htmlFile << "           window.onload = function() {" << endl;
    htmlFile << "           var canvas = document.getElementById('graphCanvas');" << endl;
    htmlFile << "           var ctx = canvas.getContext('2d');" << endl;

    // Calculate the x-position of the y-axis
    htmlFile << "           var minX = " << minX << ";" << endl;
    htmlFile << "           var maxX = " << maxX << ";" << endl;
    htmlFile << "           var xRange = " << xRange << ";" << endl;
    htmlFile << "           var minY = " << minY << ";" << endl;
    htmlFile << "           var maxY = " << maxY << ";" << endl;
    htmlFile << "           var yRange = " << yRange << ";" << endl;
    htmlFile << "           var SVG_SIZE = " << SVG_SIZE << ";" << endl;
    htmlFile << "           var hexLineColor = '" << hexLineColor << "';" << endl;

    // Draw x-axis
    htmlFile << "           ctx.beginPath();" << endl;
    htmlFile << "           ctx.moveTo(0," << (SVG_SIZE - (maxY / yRange * SVG_SIZE)) << ");" << endl;
    htmlFile << "           ctx.lineTo(" << SVG_SIZE << "," << (SVG_SIZE - (maxY / yRange * SVG_SIZE)) << ");" << endl;
    htmlFile << "           ctx.strokeStyle = 'black';" << endl;
    htmlFile << "           ctx.stroke();" << endl;

    // Draw y-axis
    htmlFile << "           ctx.beginPath();" << endl;
    htmlFile << "           ctx.moveTo(" << (-minX / xRange * SVG_SIZE) << ",0);" << endl;
    htmlFile << "           ctx.lineTo(" << (-minX / xRange * SVG_SIZE) << "," << SVG_SIZE << ");" << endl;
    htmlFile << "           ctx.strokeStyle = 'black';" << endl;
    htmlFile << "           ctx.stroke();" << endl;

    // Plot points and lines
    htmlFile << "           ctx.beginPath();" << endl;
    htmlFile << "           ctx.strokeStyle = hexLineColor;" << endl;
    htmlFile << "           ctx.lineWidth = 2;" << endl;

    for (int i = 1; i < numPoints; ++i) {
        // Check if both x-coordinates are not NaN
        if (!isnan(points[i - 1].x) && !isnan(points[i].x)) {
            // Check if both y-coordinates are not NaN
            if (!isnan(points[i - 1].y) && !isnan(points[i].y)) {
                // Draw line between two points
                htmlFile << "       ctx.moveTo(" << points[i - 1].x << "," << SVG_SIZE - points[i - 1].y << ");" << endl;
                htmlFile << "       ctx.lineTo(" << points[i].x << "," << SVG_SIZE - points[i].y << ");" << endl;
            }
        }
    }

    htmlFile << "           ctx.stroke();" << endl;

    htmlFile << "           };" << endl;
    htmlFile << "       </script>" << endl;
    htmlFile << "   </head>" << endl;
    htmlFile << "   <body>" << endl;

    // Create canvas for drawing
    htmlFile << "       <canvas id=\"graphCanvas\" width=\"" << SVG_SIZE << "\" height=\"" << SVG_SIZE << "\"></canvas>" << endl;
    htmlFile << "   </body>" << endl;
    htmlFile << "</html>" << endl;
    // Close the file
    htmlFile.close();
    cout << "Made in graph.html" << endl;
}

/****************
-----Solving-----
*****************/
Complex_Number evaluateFunction(Multi_Variable_Complex_Function function, Complex_Number x, Complex_Number y) { return function(x, y); }

string solveSingleInequality(Function function, double start, double end, double stepSize, double targetValue, string direction) {
    string result = "";
    int decimals = 0;

    if (direction == ">") {
        for (double x = start; x < end; x += stepSize) {
            if (function(x) > targetValue) {
                decimals = maxDecimals - to_string_with_precision(targetValue + 1, maxDecimals).find('.') - 1;
                result = "x > " + to_string_with_precision(targetValue + 1, decimals);
                break;
            }
        }
    } else if (direction == "<") {
        for (double x = end; x > start; x -= stepSize) {
            if (function(x) < targetValue) {
                decimals = maxDecimals - to_string_with_precision(targetValue + 1, maxDecimals).find('.') - 1;
                result = "x < " + to_string_with_precision(targetValue + 1, decimals);
                break;
            }
        }
    }

    return result;
}

#endif //starterIncludeFile