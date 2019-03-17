  # AppSettings Sample
  
  AppSettings is a simple example of using ObjectVariantMapper to map objects to/from JSON, 
  combined with OptionsFile utility class to store JSON content in the file system (managing version/format upgrades etc),
  along with ModuleGetterSetter to manage all this on a per-module basis (so settings can be defined/saved per module or globally)
  and accessed automatically in a threadsafe manner (automatically synchronized).
