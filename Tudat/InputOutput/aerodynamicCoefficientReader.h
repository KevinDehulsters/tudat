/*    Copyright (c) 2010-2016, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#include <map>
#include <iostream>

#include "Tudat/Basics/utilities.h"

#include "Tudat/InputOutput/multiDimensionalArrayReader.h"

namespace tudat
{

namespace input_output
{

template< int NumberOfDimensions >
typename boost::multi_array< double ,NumberOfDimensions >::index getIndex(
        const typename boost::multi_array< double, NumberOfDimensions >& m, const double* requestedElement,
        const unsigned short int direction)
{
    int offset = requestedElement - m.origin( );
    return( offset / m.strides( )[ direction] % m.shape( )[ direction ] +  m.index_bases( )[direction] );
}

inline boost::array< boost::multi_array< double, 1 >::index, 1 > getMultiArrayIndexArray(
        const boost::multi_array< double, 1 >& m, const double* requestedElement )
{
    typedef boost::multi_array< double, 1 > NMultiArray;
    boost::array< NMultiArray::index, 1 >  currentIndices;
    for ( unsigned int dir = 0; dir < 1; dir++ )
    {
        currentIndices[ dir ] = getIndex< 1 >( m, requestedElement, dir );
    }

    return currentIndices;
}

inline boost::array< boost::multi_array< double, 2 >::index, 2 > getMultiArrayIndexArray(
        const boost::multi_array< double, 2 >& m, const double* requestedElement )
{
    typedef boost::multi_array< double, 2 > NMultiArray;
    boost::array< NMultiArray::index, 2 >  currentIndices;
    for ( unsigned int dir = 0; dir < 2; dir++ )
    {
        currentIndices[ dir ] = getIndex< 2 >( m, requestedElement, dir );
    }

    return currentIndices;
}

inline boost::array< boost::multi_array< double, 3 >::index, 3 > getMultiArrayIndexArray(
        const boost::multi_array< double, 3 >& m, const double* requestedElement )
{
    typedef boost::multi_array< double, 3 > NMultiArray;
    boost::array< NMultiArray::index, 3 >  currentIndices;
    for ( unsigned int dir = 0; dir < 3; dir++ )
    {
        currentIndices[ dir ] = getIndex< 3 >( m, requestedElement, dir );
    }

    return currentIndices;
}


//! Function to merge three double multi-arrays of N dimension into a single Vector3d multi-array
/*!
 *  Function to merge three double multi-arrays of N dimension into a single Vector3d multi-array, where the three
 *  double multi-arrays represent the x-, y- and z-components of the Vector3ds.
 *  \param xComponents Multi-array containing the x-components of the Vector3d
 *  \param yComponents Multi-array containing the y-components of the Vector3d
 *  \param zComponents Multi-array containing the z-components of the Vector3d
 *  \return Single multi-array containing Vector3ds according to double multi-arrays.
 */
template< int NumberOfDimensions >
boost::multi_array< Eigen::Vector3d, NumberOfDimensions > mergeNDimensionalCoefficients(
        boost::multi_array< double, NumberOfDimensions > xComponents,
        boost::multi_array< double, NumberOfDimensions > yComponents,
        boost::multi_array< double, NumberOfDimensions > zComponents )
{
    boost::multi_array< Eigen::Vector3d, NumberOfDimensions > vectorArray;

    // Check input consistency
    for( unsigned int i = 0; i < NumberOfDimensions; i++ )
    {
        if( !( xComponents.shape( )[ i ] == yComponents.shape( )[ i ] ) ||
                !( xComponents.shape( )[ i ] == zComponents.shape( )[ i ] ) )
        {
            throw std::runtime_error( "Error when creating N-D merged multi-array, input sizes are inconsistent" );
        }
    }

    std::vector< size_t > sizeVector;
    const size_t* arrayShape = xComponents.shape( );
    sizeVector.assign( arrayShape, arrayShape+ xComponents.num_dimensions( ) );

    vectorArray.resize( sizeVector );


    int numberOfEntries = xComponents.num_elements( );
    Eigen::Vector3d* vectorVector = new Eigen::Vector3d[ numberOfEntries ] ;

    typedef double tValue;
    typedef boost::multi_array<tValue,NumberOfDimensions> tArray;
    typedef typename tArray::index tIndex;
    typedef boost::array<tIndex, NumberOfDimensions> tIndexArray;


    tValue* p = xComponents.data();
    tIndexArray index;
    for( unsigned int i = 0; i < numberOfEntries; i++ )
    {
        index = getMultiArrayIndexArray( xComponents, p );

        vectorVector[ i ] = ( Eigen::Vector3d( )<<xComponents( index ), yComponents( index ), zComponents( index ) ).finished( );
        ++p;
    }

    vectorArray.assign( vectorVector, vectorVector + numberOfEntries );

    delete[ ] vectorVector;

    return vectorArray;
}

//! Function to compare if two lists of aerodynamic coefficient independent variables are equal
/*!
 * Function to compare if two lists of aerodynamic coefficient independent variables (vector of vector of doubles) are equal
 * \param list1 First list that is to be compared.
 * \param list2 Second list that is to be compared.
 * \return True of the two lists are completely equal in size and contents, false otherwise.
 */
bool compareIndependentVariables( const std::vector< std::vector< double > >& list1,
                                  const std::vector< std::vector< double > >& list2 );


//! Function to read a list of aerodynamic coefficients and associated independent variables from a set of files
/*!
 *  Function to read a list of aerodynamic coefficients of 2 independent variables and associated independent variables
 *  from a set of files.
 *  \param fileNames Vector of size 3, containing the file names for the x-, y- and z- components of the aerodynamic
 *  coefficients. Note that the independent variables for each components must be identical.
 *  \return  Pair: first entry containing multi-array of aerodynamic coefficients, second containing list of independent
 *  variables at which coefficients are defined.
 */
template< int NumberOfDimensions >
std::pair< boost::multi_array< Eigen::Vector3d, NumberOfDimensions >, std::vector< std::vector< double > > >
readAerodynamicCoefficients( const std::vector< std::string >& fileNames )
{
    if( fileNames.size( ) != 3 )
    {
        throw std::runtime_error( "Error when reading 1-Dimensional aeroynamic coefficients, wrong number of files" );
    }

    std::map< int, std::string > fileNameMap;
    for( unsigned int i = 0; i < 3; i++ )
    {
        fileNameMap[ i ] = fileNames.at( i );
    }

    return readAerodynamicCoefficients< NumberOfDimensions >( fileNameMap );
}

template< int NumberOfDimensions >
std::pair< boost::multi_array< Eigen::Vector3d, NumberOfDimensions >, std::vector< std::vector< double > > >
readAerodynamicCoefficients( const std::map< int, std::string >& fileNames )
{
    // Read data from files.
    std::map< int, boost::multi_array< double, NumberOfDimensions > > rawCoefficientArrays;

    std::vector< boost::multi_array< double, NumberOfDimensions > > coefficientArrays;
    std::vector< std::vector< double > > independentVariables;

    for( std::map< int, std::string >::const_iterator fileIterator = fileNames.begin( ); fileIterator != fileNames.end( );
         fileIterator++ )
    {
        std::pair< boost::multi_array< double, NumberOfDimensions >, std::vector< std::vector< double > > > currentCoefficients =
                MultiArrayFileReader< NumberOfDimensions >::readMultiArrayAndIndependentVariables( fileIterator->second );
        if( rawCoefficientArrays.size( ) == 0 )
        {
            independentVariables = currentCoefficients.second;

        }
        else
        {
            bool areIndependentVariablesEqual = compareIndependentVariables(
                        independentVariables, currentCoefficients.second );

            if( !areIndependentVariablesEqual )
            {
                throw std::runtime_error( "Error when reading 1-Dimensional aeroynamic coefficients, inconsistent aerodynamic coefficients" );
            }
        }
        utilities::copyMultiArray< double, NumberOfDimensions >(
                    currentCoefficients.first, rawCoefficientArrays[ fileIterator->first ] );

    }

    if( rawCoefficientArrays.size( ) == 0 )
    {
        throw std::runtime_error( "Error when reading aerodynamic coefficients, no files read" );
    }
    else
    {
        coefficientArrays.resize( 3 );
        boost::multi_array< double, NumberOfDimensions > firstMultiArray = rawCoefficientArrays.begin( )->second;

        for( unsigned int i = 0; i < 3; i++ )
        {
            if( rawCoefficientArrays.count( i ) != 0 )
            {
                utilities::copyMultiArray< double, NumberOfDimensions >(
                            rawCoefficientArrays.at( i ), coefficientArrays[ i ] );
            }
            else
            {
                std::vector< size_t > sizeVector;
                const size_t* arrayShape = firstMultiArray.shape( );
                sizeVector.assign( arrayShape, arrayShape+ firstMultiArray.num_dimensions( ) );

                coefficientArrays[ i ].resize( sizeVector );

                std::fill( coefficientArrays[ i ].data( ), coefficientArrays[ i ].data() + coefficientArrays[ i ].num_elements( ), 0.0 );
            }
        }
    }

    // Merge coefficient entries
    return std::make_pair(
                mergeNDimensionalCoefficients< NumberOfDimensions >(
                    coefficientArrays.at( 0 ), coefficientArrays.at( 1 ), coefficientArrays.at( 2 ) ), independentVariables );
}

}

}
