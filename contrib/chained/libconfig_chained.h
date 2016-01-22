
#include <libconfig.h++>


namespace libconfig 
{
	template<typename T>
	static T GetDefaultValue()
	{
		return (T)0;
	}

	static std::string GetDefaultValue()
	{
		return "";
	}

	class ChainedSetting
	{
	public:

		template<typename T>
		ChainedSetting& defaultValue(T defaultValue)
		{
			// TODO: support default value;
			return *this;
		}

		ChainedSetting(Setting& setting)
			: setting(setting)
		{
		}

		template<typename T>
		static T GetDefaultValue()
		{
			return (T)0; // TODO: return default value
		}

		template<typename T>
		operator T() const
		{
			try
			{
				T value = setting;
				return value;
			}
			catch (const SettingTypeException& tex)
			{
				std::cerr << "'" << tex.getPath() << "' setting is of wrong type" << std::endl;
			}
			return GetDefaultValue<T>();
		}

		ChainedSetting operator[](const char *name) const
		{
			try
			{
				return ChainedSetting(setting[name]);
			}
			catch (const SettingNotFoundException& nfex)
			{
				std::cerr << "No " << nfex.getPath() << " setting in configuration file." << std::endl;
				return ChainedSetting(setting); // TODO: return empty settings
			}
		}

		inline ChainedSetting operator[](const std::string &name) const
		{
			return(operator[](name.c_str()));
		}
		
	private: 
		Setting& setting;
	};
}