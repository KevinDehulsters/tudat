#include "Tudat/SimulationSetup/createMassRateModels.h"

namespace tudat
{

namespace simulation_setup
{


boost::shared_ptr< basic_astrodynamics::MassRateModel >
createMassRateModel(
        const std::string& bodyWithMassRate,
        const boost::shared_ptr< MassRateModelSettings > massRateModelSettings,
        const NamedBodyMap& bodyMap,
        const basic_astrodynamics::AccelerationMap& accelerationModels )
{
    boost::shared_ptr< basic_astrodynamics::MassRateModel > massRateModel;
    switch( massRateModelSettings->massRateType_ )
    {
    case basic_astrodynamics::custom_mass_rate_model:
    {
        boost::shared_ptr< CustomMassRateModelSettings > customMassRateModelSettings =
                boost::dynamic_pointer_cast< CustomMassRateModelSettings >( massRateModelSettings );
        if( customMassRateModelSettings == NULL )
        {
            throw std::runtime_error( "Error when making cusom mass rate model, input is inconsistent" );
        }
        else
        {
            massRateModel = boost::make_shared< basic_astrodynamics::CustomMassRateModel >(
                        customMassRateModelSettings->massRateFunction_ );
        }
        break;
    }
    case basic_astrodynamics::from_thrust_mass_rate_model:
    {
        boost::shared_ptr< FromThrustMassModelSettings > fromThrustMassModelSettings =
                boost::dynamic_pointer_cast< FromThrustMassModelSettings >( massRateModelSettings );
        if( fromThrustMassModelSettings == NULL )
        {
            throw std::runtime_error( "Error when making from-engine mass rate model, input is inconsistent" );
        }
        else
        {
            std::vector< boost::shared_ptr< basic_astrodynamics::AccelerationModel3d > >
                    thrustAccelerations;
            if( accelerationModels.count( bodyWithMassRate ) != 0 )
            {
                if( accelerationModels.at( bodyWithMassRate ).count( bodyWithMassRate ) != 0 )
                {
                    thrustAccelerations = basic_astrodynamics::getAccelerationModelsOfType(
                                accelerationModels.at( bodyWithMassRate ).at( bodyWithMassRate ),
                                basic_astrodynamics::thrust_acceleration );
                }
            }

            if( thrustAccelerations.size( ) == 0 )
            {
                std::cerr<<"Warning when making from-thrust mass-rate model, no thrust model is found"<<std::endl;
            }

            if( fromThrustMassModelSettings->useAllThrustModels_ != 1 )
            {
                std::cerr<<"Warning when making from-thrust mass-rate model, single engine thrust not yet implemented"<<std::endl;

            }

            std::vector< boost::shared_ptr< basic_astrodynamics::ThrustAcceleration > >
                    explicitThrustAccelerations;
            for( unsigned int i = 0; i < thrustAccelerations.size( ); i++ )
            {
                explicitThrustAccelerations.push_back( boost::dynamic_pointer_cast< basic_astrodynamics::ThrustAcceleration >(
                                                 thrustAccelerations.at( i ) ) );
            }

            massRateModel = boost::make_shared< basic_astrodynamics::FromThrustMassRateModel >(
                        explicitThrustAccelerations );
        }
        break;
    }
    default:
        throw std::runtime_error( "Error when making mass rate model, type not recognized" );

    }
    return massRateModel;
}


std::map< std::string, std::vector< boost::shared_ptr< basic_astrodynamics::MassRateModel > > > createMassRateModelsMap(
        const NamedBodyMap& bodyMap,
        const std::map< std::string, std::vector< boost::shared_ptr< MassRateModelSettings > > >& massRateModelSettings,
        const basic_astrodynamics::AccelerationMap& accelerationModels )
{

}

} // namespace simulation_setup

} // namespace tudat

