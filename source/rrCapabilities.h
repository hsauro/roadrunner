#ifndef rrCapabilitiesH
#define rrCapabilitiesH
#include <vector>
#include "rrExporter.h"
#include "rrCapability.h"
#include "rrStringList.h"
//---------------------------------------------------------------------------

namespace rr
{
using std::vector;

/**
 * @internal
 * @deprecated
 */
class RR_DECLSPEC Capabilities
{
    protected:
        string                          mName;
        string                          mDescription;
        vector<Capability*>             mCapabilities;

    public:
                                        Capabilities(const string& name, const string& description);
        void                            add(Capability& capability);

        StringList                      asStringList();
        u_int                           count();
        void                            clear();
        Capability*                     operator[](int i);
        Capability*                     get(const string& capName);
        bool                            setParameter(const string& name, const string& value);
};

}
#endif


