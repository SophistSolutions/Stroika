# Quick-Start Building Stroika inside docker

~~~bash
mkdir Sandbox && cd Sandbox
git clone https://github.com/SophistSolutions/Stroika.git
cd Stroika
make all -j10
~~~

`make all -j10` builds all defined (default) configurations.

Alternatively, you can replace the last step:
~~~bash
make all -j10
~~~

with:
~~~bash
make default-configurations
~~~
- review the created configurations in `ConfigurationFiles` - perhaps making more, or different ones.

then
~~~bash
make CONFIGURAITON=YOUR-SELECTED-CONFIGURATION all -j10
~~~
to build just that one configuration (building one is faster than building many).

OR
~~~bash
make TAGS=Unix all -j10
~~~
to build all configurations with the tag Unix

To learn more about Building Stroika-based applications and configurations, see
[StroikaRoot/Documentation/Building Stroika.md](https://github.com/SophistSolutions/Stroika/blob/V2.1-Release/Documentation/Building%20Stroika.md)