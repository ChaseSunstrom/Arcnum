#ifndef SPARK_MATH_CONSTANTS_HPP
#define SPARK_MATH_CONSTANTS_HPP

#include "../spark.hpp"

namespace Spark
{
	namespace Math
	{
    constexpr f64 PLANCK_TIME= 5.391e-44;                        /*Seconds*/
    constexpr f64 PLANCK_LENGTH = 1.616228369772e-35; /*meters*/
    constexpr f64 REDUCED_PLANCK = 1.05457187e-34;               /*Joule seconds*/
    constexpr f64 PLANCK_CONSTANT = 6.62607015e-34;   /*Joule seconds*/
    constexpr f64 BOLTZMANN_CONSTANT = 1.380649e-23;                     /*Kilogram meter squared per second squared kelvin*/
    constexpr f64 GRAVITATIONAL_CONSTANT = 6.6743e-11;        /*Meters cubed per kilogram second squared*/
    constexpr f64 STEFAN_BOLTZMANN = 5.67e-8;         /*Watts per meter squared kelvin to the fourth*/
    constexpr f64 WIEN_WAVE_LENGTH = 2.897771955e-3;   /*kelvin meters*/
    constexpr f64 FINE_STRUCTURE = 7.29735257e-3;
    constexpr f64 CHAMPERNOWNE =
        0.123456789101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354; /*Just
                                                                                                                  goes
                                                                                                                  123456...*/
    constexpr f64 OMEGA =
        0.5671432904097838729999686622103555497538157871865125081351310792230457930866845666932194469617522946; /*W(1)*/
    constexpr f64 EULER_MASCHERONI =
        0.577215664901532860606512090082402431042159335939923598805767234884867726777664670936947063291746749;
    constexpr f64 IDEAL_GAS = 0.08205736608096060876445783215591213724670410156250; /*Atmosphere liters per mole kelvin*/
    constexpr f64 CATALAN =
        0.915965594177219015054603514932384110774149374281672134266498119621763019776254769479356512926115106;
    constexpr f64 APERNY =
        1.202056903159594285399738161511449990764986292340498881792271555341838205786313090186455873609335258;
    constexpr f64 PHI =
        1.618033988749894848204586834365638117720309179805762862135448622705260462818902449707207204189391137;
    constexpr f64 EULER =
        2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427;
    constexpr f64 PI =
        3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067;
    constexpr f64 khinchin =
        2.685452001065306445309714835481795693820382293994462953051152345557218859537152002801141174931847698;
    constexpr f64 GelfondSchneider =
        2.665144142690225188650297249873139848274211313714659492835979593364920446178705954867609180005196416;
    constexpr f64 RYDBERG = 1.0973731568e7;    /*Reciprocal meters*/
    constexpr f64 LIGHT_SECOND = 2.998e8;       /*Meters per second*/
    constexpr f64 COLOUMB = 8.98755179e9;      /*Newton square meters per coloumb squared*/
    constexpr f64 LIGHT_MINUTE = 1.799e10;      /*Meters per minute*/
    constexpr f64 WIEN_FREQUENCY = 5.878926e10; /*Hertz per kelvin*/
    constexpr f64 ATRONOMICAL_UNIT = 1.496e11; /*Meters*/
    constexpr f64 LIGHT_HOUR = 1.079e12;        /*Meters per hour*/
    constexpr f64 LIGHT_DAY = 2.59e13;          /*Meters per day*/
    constexpr f64 LIGHT_YEAR = 9.461e15;        /*Meters per year*/
    constexpr f64 PARSEC = 3.086e16;           /*Meters*/
    constexpr f64 MEGAPARSEC = 3.086e22;       /*Meters*/
    constexpr f64 AVAGARDO = 6.022e23;         /*Number of atoms or molecules in a mole*/
    constexpr f64 GIGAPARSEC = 3.086e25;       /*Meters*/
    constexpr f64 TETRAPARSEC = 3.086e28;      /*Meters*/
    constexpr f64 PETAPARSEC = 3.086e31;       /*Meters*/
    constexpr f64 YOTTAPARSEC = 3.086e40;      /*Meters*/
	}
}

#endif