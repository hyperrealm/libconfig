# method chained libconfig #

**Exception free + header only + method chained + config files**

Provides reading the configuration and defining the configuration specification at once.

```C++
#include <libconfig_chained.h>

using namespace std;
using namespace libconfig;

int main(int argc, char **argv)
{
    Config cfg;
    cfg.readFile("example.cfg");
    ChainedSetting cs(cfg.getRoot());

    string name = cs["name"].defaultValue("<name>").isMandatory();
    string abstract = cs["abstract"].defaultValue("<unknown>");
    double longitude = cs["longitude"].min(-180.0).max(180.0).isMandatory();
    double latitude = cs["latitude"].min(-90.0).max(90.0).isMandatory();

    if (cs.isAnyMandatorySettingMissing())
    {
        cerr << "Cannot proceed until all mandatory settings are set." << endl;
    }
}
```

```tty
'longitude' setting is out of valid bounds (max: 180). Value was: 1200.35
Missing 'latitude' setting in configuration file.
cannot proceed until all mandatory settings are set.
```

### Features ###

 * default values
 * limits (min/max)
 * mandatory/optional values
 * help text output for expected config format

