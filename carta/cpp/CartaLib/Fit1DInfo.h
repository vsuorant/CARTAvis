/**
 * Stores parameters & data for fitting a 1D curve.
 */
#pragma once

#include <QString>

namespace Carta
{
namespace Lib
{
class Fit1DInfo
{
public:
    /// Status of fitting the data
    enum class StatusType {
        NOT_DONE,
        ERROR,
        PARTIAL,
        COMPLETE,
        OTHER
    };

    /**
     * Constructor.
     */
    Fit1DInfo();

    /**
     * Return the y-data values that will be fit.
     */
    std::vector<double> getData() const;

    /**
     * Return the number of Gaussians to fit.
     * @return - the number of Gaussians to fit to the curve data.
     */
    int getGaussCount() const;

    /**
     * Return the number of terms in the polynomial to fit to the data.
     * @return - the degree of the polynomial to fit to the data.
     */
    int getPolyDegree() const;

    /**
     * Return an identifier for the curve that is being fit.
     * @return - an idenitifer for the curve that is being fit.
     */
    QString getId() const;

    /**
     * Set the y-values of the 1-dimensional curve that will be
     * fit.
     * @param data - the y-values of the curve to be fit.
     */
    void setData( const std::vector<double>& data );

    /**
     * Set the number of Gaussians to fit to the curve.
     * @param gaussCount - the number of Gaussians to fit to the curve.
     */
    void setGaussCount( int gaussCount );

    /**
     * Set the degree of the polynomial to fit to the data.
     * @param polyDegree - the degree of the polynomial to fit to the data.
     */
    void setPolyDegree( int polyDegree );

    /**
     * Set an identifier for the curve to be fit.
     * @param id - an identifier for the curve to be fit.
     */
    void setId( const QString& id );


    virtual ~Fit1DInfo();

private:
    QString m_id;
    int m_polyDegree;
    int m_gaussCount;
    std::vector<double> m_data;
};

} // namespace Lib
} // namespace Carta
