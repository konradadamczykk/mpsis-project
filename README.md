# Data center cost optimization application

This project allows user to minimize a cost of building data center.
Just input your data (like min servers and other requirements)
and get an output (hopefully, in finite time).

# Data model

Description and parameters are available at: 
https://docs.google.com/document/d/1_DjOO3PhC_0tffYeBRBqgxOU_NxOj59YkSqKyrCc54M/edit

# How to use

Install COIN-OR from https://www.coin-or.org/downloading/

Install kcgi from https://kristaps.bsd.lv/kcgi/
```
./configure
make
make install
```

Install jsmn from https://github.com/zserge/jsmn
```
make 
make install
```

Install postgresql
Create user `test` password `qwerty`
Create database `omptim`
In database create table:
```
CREATE TABLE optimalization2 (
	id SERIAL primary key,
	created timestamp default current_timestamp, 
	finished default NULL,
	result varchar[300] default NULL
);
```
Enable connection from 127.0.0.1 if they're disabled.

Install libpq-dev
```
apt-get install libpq-dev
```

Install pqxx from https://github.com/jtv/libpqxx
```
./configure
make install
```


## Compilation:
```
g++ cgi.cpp -lkcgi -L jsmn -ljsmn -lCoinUtils -lOsiClp -lpqxx -lpq -o cgi -lz
```

## Apache config:

/etc/apache2/sites-available/000-default.conf - in VirtualHost section

```
ScriptAlias /cgi-bin/ "/var/www/cgi-bin/"
<Directory "/var/www/cgi-bin/">
	Options +ExecCGI
	AddHandler cgi-script .cgi
</Directory>
```

## Running

Post to 
```
http://<yourip>/cgi-bin/cgi
```

with body like
```
{
	"m2_cost":"200",
	"num_server_vendors":"2",
	"cost_server":["120","240"],
	"max_num_serv_vendor":["10","1000"],
	"monthly_elec_server_vendor":["20","10"],
	"num_rack_vendors":"2",
	"rack_surface_vendor":["14","24"],
	"rack_cost_vendor":["20","20"],
	"rack_max_num_vendor":["2400","2500"],
	"num_serv_in_rack_vendor":"2",
	"kWh_cost":"2",
	"max_m2":"2000",
	"min_all_servers":"200",
	"min_num_racks":"100",
	"max_kwh_usage":"200000"
}
```

Get to
```http://<yourip>/cgi-bin/cgi?id=<id>```
to obtain results from database

Frontend at
```http://<yourip>/frontend/```

# ENJOY