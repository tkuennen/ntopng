/*
 *
 * (C) 2013-17 - ntop.org
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "ntop_includes.h"

/* ******************************************* */

Prefs::Prefs(Ntop *_ntop) : RuntimePrefs() {
  num_deferred_interfaces_to_register = 0, cli = NULL;
  memset(deferred_interfaces_to_register, 0, sizeof(deferred_interfaces_to_register));
  ntop = _ntop, sticky_hosts = location_none, simulate_vlans = false;
  local_networks = strdup(CONST_DEFAULT_HOME_NET "," CONST_DEFAULT_LOCAL_NETS);
  local_networks_set = false, shutdown_when_done = false;
  enable_users_login = true, disable_localhost_login = false;
  enable_dns_resolution = sniff_dns_responses = true, use_promiscuous_mode = true;
  resolve_all_host_ip = false, online_license_check = false;
  max_num_hosts = MAX_NUM_INTERFACE_HOSTS, max_num_flows = MAX_NUM_INTERFACE_HOSTS;
  attacker_max_num_flows_per_sec = victim_max_num_flows_per_sec = CONST_MAX_NEW_FLOWS_SECOND;
  attacker_max_num_syn_per_sec = victim_max_num_syn_per_sec = CONST_MAX_NUM_SYN_PER_SECOND;  
  data_dir = strdup(CONST_DEFAULT_DATA_DIR);
  enable_access_log = false, flow_aggregation_enabled = false;
  enable_mac_ndpi_stats = false;

  install_dir = NULL, captureDirection = PCAP_D_INOUT;
  docs_dir = strdup(CONST_DEFAULT_DOCS_DIR);
  scripts_dir = strdup(CONST_DEFAULT_SCRIPTS_DIR);
  callbacks_dir = strdup(CONST_DEFAULT_CALLBACKS_DIR);
  prefs_dir = NULL;
  config_file_path = ndpi_proto_path = NULL;
  http_port = CONST_DEFAULT_NTOP_PORT, alt_http_port = 0;
  http_prefix = strdup(""), zmq_encryption_pwd = NULL;
  instance_name = NULL;
  categorization_enabled = false, enable_users_login = true;
  categorization_key = NULL, zmq_encryption_pwd = NULL;
  es_index = es_url = es_user = es_pwd = NULL;
  https_port = 0; // CONST_DEFAULT_NTOP_PORT+1;
  change_user = true, daemonize = false;
  user = strdup(CONST_DEFAULT_NTOP_USER);
  http_binding_address = NULL;
  https_binding_address = NULL; // CONST_ANY_ADDRESS;
  lan_interface = wan_interface = NULL;
  httpbl_key = NULL, flashstart = NULL;
  cpu_affinity = NULL;
  redis_host = strdup("127.0.0.1");
  redis_password = NULL;
  redis_port = 6379;
  redis_db_id = 0;
  dns_mode = 0;
  pid_path = strdup(DEFAULT_PID_PATH);
  packet_filter = NULL;
  num_interfaces = 0, enable_auto_logout = true;
  dump_flows_on_es = dump_flows_on_mysql = dump_flows_on_ls = false;
  read_flows_from_mysql = false;
  enable_taps = false;
  memset(ifNames, 0, sizeof(ifNames));
  dump_hosts_to_db = location_none;
  json_labels_string_format = true;
#ifdef WIN32
  daemonize = true;
#endif
  export_endpoint = NULL;
  enable_ixia_timestamps = enable_vss_apcon_timestamps = false;
  enable_user_scripts   = CONST_DEFAULT_USER_SCRIPTS_ENABLED;

  es_type = strdup((char*)"flows"), es_index = strdup((char*)"ntopng-%Y.%m.%d"),
    es_url = strdup((char*)"http://localhost:9200/_bulk"),
    es_user = strdup((char*)""), es_pwd = strdup((char*)"");

  mysql_host = mysql_dbname = mysql_tablename = mysql_user = mysql_pw = NULL;
  ls_host = NULL;
  ls_port = NULL;
  ls_proto = NULL;
  has_cmdl_trace_lvl      = false;
  has_cmdl_disable_alerts = false;
}

/* ******************************************* */

Prefs::~Prefs() {
  for(int i=0; i<num_interfaces; i++) {
    if(ifNames[i].name) free(ifNames[i].name);
    if(ifNames[i].description) free(ifNames[i].description);
  }

  if(data_dir)         free(data_dir);
  if(install_dir)      free(install_dir);
  if(docs_dir)         free(docs_dir);
  if(scripts_dir)      free(scripts_dir);
  if(callbacks_dir)    free(callbacks_dir);
  if(prefs_dir)        free(prefs_dir);
  if(config_file_path) free(config_file_path);
  if(user)             free(user);
  if(pid_path)         free(pid_path);
  if(packet_filter)    free(packet_filter);
  if(cpu_affinity)     free(cpu_affinity);
  if(es_type)          free(es_type);
  if(es_index)         free(es_index);
  if(es_url)           free(es_url);
  if(es_user)          free(es_user);
  if(es_pwd)           free(es_pwd);
  if(instance_name)    free(instance_name);
  free(http_prefix);
  free(local_networks);
  free(redis_host);
  if(redis_password)  free(redis_password);
  if(cli)             free(cli);
  if(mysql_host)      free(mysql_host);
  if(mysql_dbname)    free(mysql_dbname);
  if(mysql_tablename) free(mysql_tablename);
  if(mysql_user)      free(mysql_user);
  if(mysql_pw)        free(mysql_pw);
  if(ls_host)         free(ls_host);
  if(ls_port)	      free(ls_port);
  if(ls_proto)	      free(ls_proto);
  if(http_binding_address)  free(http_binding_address);
  if(https_binding_address) free(https_binding_address);
  if(lan_interface)	free(lan_interface);
  if(wan_interface)	free(wan_interface);
  /* NOTE: flashstart is deleted by the Ntop class */
}

/* ******************************************* */

/* C-binding needed by Win32 service call */
void nDPIusage() {
  printf("\nnDPI detected protocols:\n");

  struct ndpi_detection_module_struct *ndpi_struct = ndpi_init_detection_module();
  ndpi_dump_protocols(ndpi_struct);

  exit(0);
}

/* ******************************************* */

/* C-binding needed by Win32 service call */
void usage() {
  printf("ntopng %s v.%s - " NTOP_COPYRIGHT "\n\n"
	 "Usage:\n"
	 "  ntopng <configuration file path>\n"
	 "  or\n"
	 "  ntopng <command line options> \n\n"
	 "Options:\n"
	 "[--dns-mode|-n] <mode>              | DNS address resolution mode\n"
	 "                                    | 0 - Decode DNS responses and resolve\n"
	 "                                    |     local numeric IPs only (default)\n"
	 "                                    | 1 - Decode DNS responses and resolve all\n"
	 "                                    |     numeric IPs\n"
	 "                                    | 2 - Decode DNS responses and don't\n"
	 "                                    |     resolve numeric IPs\n"
	 "                                    | 3 - Don't decode DNS responses and don't\n"
	 "                                    |     resolve numeric IPs\n"
	 "[--interface|-i] <interface|pcap>   | Input interface name (numeric/symbolic),\n"
         "                                    | view or pcap file path\n"
#ifndef WIN32
	 "[--data-dir|-d] <path>              | Data directory (must be writable).\n"
	 "                                    | Default: %s\n"
	 "[--install-dir|-t] <path>           | Set the installation directory to <dir>.\n"
	 "                                    | Should be set when installing ntopng \n"
	 "                                    | under custom directories\n"
	 "[--daemon|-e]                       | Daemonize ntopng\n"
#endif
	 "[--httpdocs-dir|-1] <path>          | HTTP documents root directory.\n"
	 "                                    | Default: %s\n"
	 "[--scripts-dir|-2] <path>           | Scripts directory.\n"
	 "                                    | Default: %s\n"
	 "[--callbacks-dir|-3] <path>         | Callbacks directory.\n"
	 "                                    | Default: %s\n"
	 "[--prefs-dir|-4] <path>             | Preferences directory used to serialize\n"
	 "                                    | and deserialize file\n"
	 "                                    | containing runtime preferences.\n"
	 "                                    | Default: %s\n"
	 "[--no-promisc|-u]                   | Don't set the interface in promisc mode.\n"
	 "[--traffic-filtering|-k] <param>    | Filter traffic using cloud services.\n"
	 "                                    | (default: disabled). Available options:\n"
	 "                                    | httpbl:<api_key>   See README.httpbl\n"
	 "                                    | flashstart:<opt>   See README.flashstart\n"
	 "[--http-port|-w] <[addr:]port>      | HTTP. Set to 0 to disable http server.\n"
	 "                                    | Addr can be an IPv4 (192.168.1.1)\n"
	 "                                    | or IPv6 ([3ffe:2a00:100:7031::1]) addr.\n"
	 "                                    | Surround IPv6 addr with square brackets.\n"
	 "                                    | Prepend a ':' without addr before the\n"
	 "                                    | listening port on the loopback address.\n"
	 "                                    | Default port: %u\n"
	 "                                    | Examples:\n"
	 "                                    | -w :3000\n"
	 "                                    | -w 192.168.1.1:3001\n"
	 "                                    | -w [3ffe:2a00:100:7031::1]:3002\n"
	 "[--https-port|-W] <[:]https port>   | HTTPS. See also -w above. Default: %u\n"
	 "[--local-networks|-m] <local nets>  | Local nets list (default: 192.168.1.0/24)\n"
	 "                                    | (e.g. -m \"192.168.0.0/24,172.16.0.0/16\")\n"
	 "[--ndpi-protocols|-p] <file>.protos | Specify a nDPI protocol file\n"
	 "                                    | (eg. protos.txt)\n"
	 "[--redis|-r] <fmt>                  | Redis connection. <fmt> is specified as\n"
	 "                                    | [h[:port[:pwd]]][@db-id] where db-id\n"
	 "                                    | identifies the database Id (default 0).\n"
	 "                                    | h is the host running Redis (default\n"
	 "                                    | localhost), optionally followed by a\n"
	 "                                    |  ':'-separated port (default 6379).\n"
	 "                                    | A password can be specified after\n"
	 "                                    | the port when Redis auth is required.\n"
	 "                                    | By default password auth is disabled.\n"
	 "                                    | Examples:\n"
	 "                                    | -r @2\n"
	 "                                    | -r 129.168.1.3\n"
	 "                                    | -r 129.168.1.3:6379@3\n"
	 "                                    | -r 129.168.1.3:6379:nt0pngPwD@0\n"
#ifdef linux
	 "[--core-affinity|-g] <cpu core ids> | Bind the capture/processing threads to\n"
	 "                                    | specific CPU cores (specified as a comma-\n"
	 "                                    | separated list)\n"
#endif
	 "[--user|-U] <sys user>              | Run ntopng with the specified user\n"
	 "                                    | instead of %s\n"
	 "[--dont-change-user|-s]             | Do not change user (debug only)\n"
	 "[--shutdown-when-done]              | Terminate after reading the pcap (debug only)\n"
	 "--zmq-encrypt-pwd <pwd>             | Encrypt the ZMQ data using with <pwd>\n"
	 "[--disable-autologout|-q]           | Disable web logout for inactivity\n"
	 "[--disable-login|-l] <mode>         | Disable user login authentication:\n"
	 "                                    | 0 - Disable login only for localhost\n"
	 "                                    | 1 - Disable login for all hosts\n"
	 "[--max-num-flows|-X] <num>          | Max number of active flows\n"
	 "                                    | (default: %u)\n"
	 "[--max-num-hosts|-x] <num>          | Max number of active hosts\n"
	 "                                    | (default: %u)\n"
	 "[--users-file|-u] <path>            | Users configuration file path\n"
	 "                                    | Default: %s\n"
#ifndef WIN32
	 "[--pid|-G] <path>                   | Pid file path\n"
#endif

	 "[--disable-alerts|-H]               | Disable alerts generation\n"
	 "[--packet-filter|-B] <filter>       | Ingress packet filter (BPF filter)\n"
	 "[--dump-flows|-F] <mode>            | Dump expired flows. Mode:\n"
	 "                                    | es            Dump in ElasticSearch database\n"
	 "                                    |   Format:\n"
	 "                                    |   es;<idx type>;<idx name>;<es URL>;<http auth>\n"
	 "                                    |   Example:\n"
	 "                                    |   es;ntopng;ntopng-%%Y.%%m.%%d;http://localhost:9200/_bulk;\n"
	 "                                    |   Note: the <idx name> accepts the\n"
	 "                                    |   strftime() format.\n"
	 "                                    |\n"
	 "                                    | logstash      Dump in LogStash engine\n"
	 "                                    |   Format:\n"
	 "                                    |   logstash;<host>;<proto>;<port>\n"
	 "                                    |   Example:\n"
	 "                                    |   logstash;localhost;tcp;5510\n"
	 "                                    |\n"
	 "                                    | mysql         Dump in MySQL database\n"
	 "                                    |   Format:\n"
	 "                                    |   mysql;<host|socket>;<dbname>;<table name>;<user>;<pw>\n"
	 "                                    |   mysql;localhost;ntopng;flows;root;\n"
	 "                                    |\n"
	 "                                    | mysql-nprobe  Read from an nProbe-generated MySQL database\n"
	 "                                    |   Format:\n"
	 "                                    |   mysql-nprobe;<host|socket>;<dbname>;<prefix>;<user>;<pw>\n"
	 "                                    |   mysql-nprobe;localhost;ntopng;nf;root;\n"
	 "                                    |   Notes:\n"
	 "                                    |    The <prefix> must be the same as used in nProbe.\n"
	 "                                    |    Only one ntopng -i <interface> is allowed.\n"
	 "                                    |    Flows are only read. Dump is assumed to be done by nProbe.\n"
	 "                                    |   Example:\n"
	 "                                    |     ./nprobe ... --mysql=\"localhost:ntopng:nf:root:root\"\n"
	 "                                    |     ./ntopng ... --dump-flows=\"mysql-nprobe;localhost;ntopng;nf;root;root\"\n"
	 "[--export-flows|-I] <endpoint>      | Export flows with the specified endpoint\n"
	 "[--dump-hosts|-D] <mode>            | Dump hosts policy (default: none).\n"
	 "                                    | Values:\n"
	 "                                    | all    - Dump all hosts\n"
	 "                                    | local  - Dump only local hosts\n"
	 "                                    | remote - Dump only remote hosts\n"
	 "                                    | none   - Do not dump any host\n"
	 "[--sticky-hosts|-S] <mode>          | Don't flush hosts (default: none).\n"
	 "                                    | Values:\n"
	 "                                    | all    - Keep all hosts in memory\n"
	 "                                    | local  - Keep only local hosts\n"
	 "                                    | remote - Keep only remote hosts\n"
	 "                                    | none   - Flush hosts when idle\n"
	 "--hw-timestamp-mode <mode>          | Enable hw timestamping/stripping.\n"
	 "                                    | Supported TS modes are:\n"
	 "                                    | apcon - Timestamped pkts by apcon.com\n"
	 "                                    |         hardware devices\n"
	 "                                    | ixia  - Timestamped pkts by ixiacom.com\n"
	 "                                    |         hardware devices\n"
	 "                                    | vss   - Timestamped pkts by vssmonitoring.com\n"
	 "                                    |         hardware devices\n"
	 "--capture-direction                 | Specify packet capture direction\n"
	 "                                    | 0=RX+TX (default), 1=RX only, 2=TX only\n"
	 "--online-license-check              | Check license online\n"
	 "[--enable-taps|-T]                  | Enable tap interfaces for dumping traffic\n"
	 "[--enable-user-scripts]             | Enable LUA user scripts\n"
	 "[--http-prefix|-Z] <prefix>         | HTTP prefix to be prepended to URLs.\n"
	 "                                    | Useful when using ntopng behind a proxy.\n"
	 "[--instance-name|-N] <name>         | Assign a name to this ntopng instance.\n"
#ifdef NTOPNG_PRO
	 "[--community]                       | Start ntopng in community edition.\n"
	 "[--check-license]                   | Check if the license is valid.\n"
	 "[--check-maintenance]               | Check until maintenance is included\n"
	 "                                    | in the license.\n"
#endif
	 "[--verbose|-v] <level>              | Verbose tracing [0 (min).. 6 (debug)]\n"
	 "[--version|-V]                      | Print version and quit\n"
	 "--print-ndpi-protocols              | Print the nDPI protocols list\n"
	 "--simulate-vlans                    | Simulate VLAN traffic (debug only)\n"
	 "[--help|-h]                         | Help\n"
	 , PACKAGE_MACHINE, PACKAGE_VERSION,
#ifndef WIN32
	 ntop->get_working_dir(),
#endif
	 CONST_DEFAULT_DOCS_DIR, CONST_DEFAULT_SCRIPTS_DIR,
         CONST_DEFAULT_CALLBACKS_DIR,
	 CONST_DEFAULT_DATA_DIR,
	 CONST_DEFAULT_NTOP_PORT, CONST_DEFAULT_NTOP_PORT+1,
         CONST_DEFAULT_NTOP_USER,
	 MAX_NUM_INTERFACE_HOSTS, MAX_NUM_INTERFACE_HOSTS,
	 CONST_DEFAULT_USERS_FILE);

  printf("\n");

  NetworkInterface n("dummy");
  n.printAvailableInterfaces(true, 0, NULL, 0);

  exit(0);
}

/* ******************************************* */

u_int32_t Prefs::getDefaultPrefsValue(const char *pref_key, u_int32_t default_value) {
  char rsp[32];

  if(ntop->getRedis()->get((char*)pref_key, rsp, sizeof(rsp)) == 0)
    return(atoi(rsp));
  else {
    snprintf(rsp, sizeof(rsp), "%u", default_value);
    ntop->getRedis()->set((char*)pref_key, rsp);
    return(default_value);
  }
}

/* ******************************************* */

void Prefs::setTraceLevelFromRedis(){
  char lvlStr[CONST_MAX_LEN_REDIS_VALUE];
  if(!hasCmdlTraceLevel()
     && ntop->getRedis()->get((char *)CONST_RUNTIME_PREFS_LOGGING_LEVEL, lvlStr, sizeof(lvlStr)) == 0){
    if(!strcmp(lvlStr, "trace")){
      ntop->getTrace()->set_trace_level(TRACE_LEVEL_TRACE);
    }
    else if(!strcmp(lvlStr, "debug")){
      ntop->getTrace()->set_trace_level(TRACE_LEVEL_DEBUG);
    }
    else if(!strcmp(lvlStr, "info")){
      ntop->getTrace()->set_trace_level(TRACE_LEVEL_INFO);
    }
    else if(!strcmp(lvlStr, "normal")){
      ntop->getTrace()->set_trace_level(TRACE_LEVEL_NORMAL);
    }
    else if(!strcmp(lvlStr, "warning")){
      ntop->getTrace()->set_trace_level(TRACE_LEVEL_WARNING);
    }
    else if(!strcmp(lvlStr, "error")){
      ntop->getTrace()->set_trace_level(TRACE_LEVEL_ERROR);
    }
  }
}

/* ******************************************* */

void Prefs::setAlertsEnabledFromRedis(){
  char resp[CONST_MAX_LEN_REDIS_VALUE];
  if(!hasCmdlDisableAlerts()
     && ntop->getRedis()->get((char *)CONST_ALERT_DISABLED_PREFS, resp, sizeof(resp)) == 0){
    if(resp[0] == '1')
      set_alerts_status(false /* disable */);
    else
      set_alerts_status(true  /* enable */);
  }
}

/* ******************************************* */

void Prefs::getDefaultStringPrefsValue(const char *pref_key, char **buffer, const char *default_value) {
  char rsp[MAX_PATH];

  if((ntop->getRedis()->get((char*)pref_key, rsp, sizeof(rsp)) == 0) && (rsp[0] != '\0'))
    *buffer = strdup(rsp);
  else
    *buffer = strdup(default_value);
}

/* ******************************************* */

bool Prefs::getDefaultBoolPrefsValue(const char *pref_key, const bool default_value) {
  char rsp[8];
  
  if(ntop->getRedis()->get((char*)pref_key, rsp, sizeof(rsp)) == 0)
    return((rsp[0] == '1') ? true : false);
  else
    return(default_value);
}
  
/* ******************************************* */

void Prefs::reloadPrefsFromRedis() {
  // sets to the default value in redis if no key is found
  getDefaultPrefsValue(CONST_RUNTIME_IS_AUTOLOGOUT_ENABLED,
		       CONST_DEFAULT_IS_AUTOLOGOUT_ENABLED);
  // alert preferences
  enable_access_log     = getDefaultBoolPrefsValue(CONST_PREFS_ENABLE_ACCESS_LOG, false);

  setTraceLevelFromRedis();
  setAlertsEnabledFromRedis();
  refreshHostsAlertsPrefs();
  refreshLanWanInterfaces();
  refreshMacNdpiStatsPrefs();
}

/* ******************************************* */

void Prefs::loadInstanceNameDefaults() {
  // Do not re-set the interface name if it has already been set via command line
  if(instance_name || !ntop)
    return;
  else {
    char tmp[256];

    if(gethostname(tmp, sizeof(tmp)))
      ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to read hostname [%s]",
				   strerror(errno));
    else
      instance_name = strdup(tmp);
  }
}

/* ******************************************* */

static const struct option long_options[] = {
#ifndef WIN32
  { "data-dir",                          required_argument, NULL, 'd' },
  { "install-dir",                       required_argument, NULL, 't' },
#endif
  { "daemon",                            no_argument,       NULL, 'e' },
  { "core-affinity",                     required_argument, NULL, 'g' },
  { "help",                              no_argument,       NULL, 'h' },
  { "interface",                         required_argument, NULL, 'i' },
  { "local-networks",                    required_argument, NULL, 'm' },
  { "dns-mode",                          required_argument, NULL, 'n' },
  { "traffic-filtering",                 required_argument, NULL, 'k' },
  { "disable-login",                     required_argument, NULL, 'l' },
  { "ndpi-protocols",                    required_argument, NULL, 'p' },
  { "disable-autologout",                no_argument,       NULL, 'q' },
  { "redis",                             required_argument, NULL, 'r' },
  { "dont-change-user",                  no_argument,       NULL, 's' },
  { "no-promisc",                        no_argument,       NULL, 'u' },
  { "verbose",                           required_argument, NULL, 'v' },
  { "max-num-hosts",                     required_argument, NULL, 'x' },
  { "http-port",                         required_argument, NULL, 'w' },
  { "packet-filter",                     required_argument, NULL, 'B' },
  { "dump-hosts",                        required_argument, NULL, 'D' },
  { "dump-flows",                        required_argument, NULL, 'F' },
#ifndef WIN32
  { "pid",                               required_argument, NULL, 'G' },
#endif
  { "disable-alerts",                    no_argument,       NULL, 'H' },
  { "export-flows",                      required_argument, NULL, 'I' },
  { "capture-direction",                 required_argument, NULL, 'Q' },
  { "sticky-hosts",                      required_argument, NULL, 'S' },
  { "enable-taps",                       no_argument,       NULL, 'T' },
  { "user",                              required_argument, NULL, 'U' },
  { "version",                           no_argument,       NULL, 'V' },
  { "max-num-flows",                     required_argument, NULL, 'X' },
  { "https-port",                        required_argument, NULL, 'W' },
  { "http-prefix",                       required_argument, NULL, 'Z' },
  { "instance-name",                     required_argument, NULL, 'N' },
  { "httpdocs-dir",                      required_argument, NULL, '1' },
  { "scripts-dir",                       required_argument, NULL, '2' },
  { "callbacks-dir",                     required_argument, NULL, '3' },
  { "prefs-dir",                         required_argument, NULL, '4' },
  { "print-ndpi-protocols",              no_argument,       NULL, 210 },
  { "online-license-check",              no_argument,       NULL, 211 },
  { "hw-timestamp-mode",                 required_argument, NULL, 212 },
  { "shutdown-when-done",                no_argument,       NULL, 213 },
  { "simulate-vlans",                    no_argument,       NULL, 214 },
  { "zmq-encrypt-pwd",                   required_argument, NULL, 215 },
  { "enable-user-scripts",               no_argument,       NULL, 216 },
#ifdef NTOPNG_PRO
  { "check-maintenance",                 no_argument,       NULL, 252 },
  { "check-license",                     no_argument,       NULL, 253 },
  { "community",                         no_argument,       NULL, 254 },
#endif
  /* End of options */
  { NULL,                                no_argument,       NULL,  0 }
};

/* ******************************************* */

void Prefs::parseHTTPPort(char *arg) {
  char tmp[32], *a, *_t;

  snprintf(tmp, sizeof(tmp), "%s", arg);

  a = strtok_r(tmp, ",", &_t);
  if(a) {  
    http_port = atoi(a);

    a = strtok_r(NULL, ",", &_t);
    if(a)
      alt_http_port = atoi(a);
  }
}

/* ******************************************* */

int Prefs::setOption(int optkey, char *optarg) {
  char *double_dot, *p, *opt = NULL, buf[128] = { '\0' };
  int len = (optarg ? strlen(optarg) : 0)+6;

  for(int i=0; ; i++) {
    if(long_options[i].val == optkey) {
      opt = (char*)long_options[i].name;
      len += strlen(opt)+2;
      break;
    }
  }

  if((p = (char*)malloc(len)) != NULL) {
    if(opt) {
      if(optarg)
	snprintf(p, len-1, "--%s \"%s\" ", opt, optarg);
      else
	snprintf(p, len-1, "--%s ", opt);
    } else {
      if(optarg)
	snprintf(p, len-1, "-%c %s ", optkey, optarg);
      else
	snprintf(p, len-1, "-%c ", optkey);
    }

    if(cli == NULL)
      cli = p;
    else {
      int l = strlen(cli);
      char *backup = cli;

      if((cli = (char*)realloc(cli, l+len)) != NULL)
	strcpy(&cli[l], p);
      else
	cli = backup;
    }
  }

  switch(optkey) {
  case 'B':
    if((optarg[0] == '\"') && (strlen(optarg) > 2)) {
      packet_filter = strdup(&optarg[1]);
      packet_filter[strlen(packet_filter)-1] = '\0';
    } else
      packet_filter = strdup(optarg);
    break;

  case 'k':
    if(strncmp(optarg, HTTPBL_STRING, strlen(HTTPBL_STRING)) == 0)
      httpbl_key = strdup(&optarg[strlen(HTTPBL_STRING)]);
    else if(strncmp(optarg, FLASHSTART_STRING, strlen(FLASHSTART_STRING)) == 0) {
      char *tmp, *user = strtok_r(&optarg[strlen(FLASHSTART_STRING)], ":", &tmp), *pwd = NULL;

      if(user) pwd = strtok_r(NULL, ":", &tmp);

      if(user && pwd) {
	char *alt_dns;
	char *alt_port;

	if((alt_dns = strtok_r(NULL, ":", &tmp)) != NULL)
	  alt_port = strtok_r(NULL, ":", &tmp);
	else
	  alt_port = NULL;
	   
	if((flashstart = new Flashstart(user, pwd, alt_dns, alt_port ? atoi(alt_port) : 53, false)) != NULL)
	  ntop->set_flashstart(flashstart);
      } else
	ntop->getTrace()->traceEvent(TRACE_WARNING, "Invalid format for --%s<user>:<pwd>",
				     FLASHSTART_STRING);
    } else
      ntop->getTrace()->traceEvent(TRACE_ERROR, "Unknown value %s for -k", optarg);
    break;

  case 'u':
    use_promiscuous_mode = false;
    break;

#ifndef WIN32
  case 'd':
    ntop->setWorkingDir(optarg);
    break;

  case 't':
    install_dir = strndup(optarg, MAX_PATH);
    break;
#endif

  case 'D':
    if(!strcmp(optarg, "all")) dump_hosts_to_db = location_all;
    else if(!strcmp(optarg, "local")) dump_hosts_to_db = location_local_only;
    else if(!strcmp(optarg, "remote")) dump_hosts_to_db = location_remote_only;
    else if(!strcmp(optarg, "none")) dump_hosts_to_db = location_none;
    else ntop->getTrace()->traceEvent(TRACE_ERROR, "Unknown value %s for -D", optarg);
    break;

  case 'e':
    daemonize = true;
    break;

  case 'S':
    if(!strcmp(optarg, "all")) sticky_hosts = location_all;
    else if(!strcmp(optarg, "local"))  sticky_hosts = location_local_only;
    else if(!strcmp(optarg, "remote")) sticky_hosts = location_remote_only;
    else if(!strcmp(optarg, "none"))   sticky_hosts = location_none;
    else ntop->getTrace()->traceEvent(TRACE_ERROR, "Unknown value %s for -S", optarg);
    break;

  case 'g':
    cpu_affinity = strdup(optarg);
    break;

  case 'm':
    free(local_networks);
    local_networks = strdup(optarg);
    local_networks_set = true;
    break;

  case 'n':
    dns_mode = atoi(optarg);
    switch(dns_mode) {
    case 0:
      break;
    case 1:
      resolve_all_hosts();
      break;
    case 2:
      disable_dns_resolution();
      break;
    case 3:
      disable_dns_resolution();
      disable_dns_responses_decoding();
      break;
    default:
      usage();
    }
    break;

  case 'p':
    ndpi_proto_path = strdup(optarg);
    ntop->setCustomnDPIProtos(ndpi_proto_path);
    break;

  case 'q':
    enable_auto_logout = false;
    break;

  case 'Q':
    switch(atoi(optarg)) {
    case 1:  setCaptureDirection(PCAP_D_IN);    break;
    case 2:  setCaptureDirection(PCAP_D_OUT);   break;
    default: setCaptureDirection(PCAP_D_INOUT); break;
    }
    break;

  case 'T':
    enable_taps = true;
    break;

  case 'h':
    ntop->registerPrefs(this, true);
    help();
    break;

  case 'i':
    if(num_deferred_interfaces_to_register < MAX_NUM_INTERFACES)
      deferred_interfaces_to_register[num_deferred_interfaces_to_register++] = strdup(optarg);
    else
      ntop->getTrace()->traceEvent(TRACE_WARNING, "Too many interfaces specified with -i: ignored %s", optarg);
    break;

  case 'w':
    if(strchr(optarg, ':') == NULL) {
      // only the port
      parseHTTPPort(optarg);
    } else if(optarg[0] == ':'){
      // first char == ':' binds to the loopback address
      parseHTTPPort(&optarg[1]);
      bind_http_to_loopback();
    } else {
      // ':' is after the first character, so
      // we need to parse both the ip address and the port
      double_dot = strrchr(optarg, ':');
      u_int len = double_dot - optarg;
      http_binding_address = strndup(optarg, len);
      parseHTTPPort(&double_dot[1]);
    }
    break;

  case 'W':
    if(strchr(optarg, ':') == NULL){
      // only the port
      https_port = atoi(optarg);
    } else if(optarg[0] == ':'){
      // first char == ':' binds to the loopback address
      https_port = atoi(&optarg[1]);
      bind_https_to_loopback();
    } else {
      // ':' is after the first character, so
      // we need to parse both the ip address and the port
      double_dot = strrchr(optarg, ':');
      u_int len = double_dot - optarg;
      https_binding_address = strndup(optarg, len);
      https_port = atoi(&double_dot[1]);
    }
    break;

  case 'Z':
    if(optarg[0] != '/') {
      ntop->getTrace()->traceEvent(TRACE_WARNING, "-Z argument (%s) must begin with '/' (example /ntopng): skipped", optarg);
    } else {
      int len = strlen(optarg);

      if(len > 0) {
	if(optarg[len-1] == '/') {
	  ntop->getTrace()->traceEvent(TRACE_WARNING, "-Z argument (%s) cannot end with '/' (example /ntopng): skipped", optarg);
	} else {
	  free(http_prefix);
	  http_prefix = strdup(optarg);
	}
      }
    }
    break;

  case 'N':
    instance_name = strndup(optarg, 256);
    break;

  case 'r':
    {
      char *r;

      /*
	Supported formats for --redis

	host:port
	host@redis_instance
	host:port@redis_instance
       */
      snprintf(buf, sizeof(buf), "%s", optarg);
      r = strrchr(buf, '@');
      if(r) {
	redis_db_id = atoi((const char*)&r[1]);
	(*r) = '\0';
      }

      if(strchr(buf, ':')) {
	char *w, *c;

	c = strtok_r(buf, ":", &w);

	if(redis_host) free(redis_host);
	redis_host = strdup(c);

	c = strtok_r(NULL, ":", &w);
	if(c) redis_port = atoi(c);

	c = strtok_r(NULL, ":", &w);
	if(c) redis_password = strdup(c);
      } else if(strlen(buf) > 0) {
	/* only the host */
	if(redis_host) free(redis_host);
	redis_host = strdup(buf);
      }

      ntop->getTrace()->traceEvent(TRACE_NORMAL,
				   "ntopng will use redis %s:%u@%u",
				   redis_host, redis_port, redis_db_id);
      if(redis_password)
	ntop->getTrace()->traceEvent(TRACE_NORMAL,
				     "redis connection is password-protected");
    }
    break;

  case 's':
    change_user = false;
    break;

  case '1':
    free(docs_dir);
    docs_dir = strdup(optarg);
    break;

  case '2':
    free(scripts_dir);
    scripts_dir = strdup(optarg);
    break;

  case '3':
    free(callbacks_dir);
    callbacks_dir = strdup(optarg);
    break;

  case '4':
    if(prefs_dir) free(prefs_dir);
    prefs_dir = strdup(optarg);
    break;

  case 'l':
    switch(atoi(optarg)) {
    case 0:
      disable_localhost_login = true;
      ntop->getTrace()->traceEvent(TRACE_NORMAL, "Localhost HTTP user login disabled");
      break;
    case 1:
      enable_users_login = false;
      ntop->getTrace()->traceEvent(TRACE_NORMAL, "All HTTP user login disabled");
      break;
    default:
      ntop->getTrace()->traceEvent(TRACE_ERROR,
				   "Invalid '%s' value specified for -l: ignored",
				   optarg);
    }
    break;
  case 'x':
    max_num_hosts = max_val(atoi(optarg), 1024);
    break;

  case 'v':
    {
      has_cmdl_trace_lvl = true;
      errno = 0;
      int8_t lvl = (int8_t)strtol(optarg, NULL, 10);
      if(errno) {
	ntop->getTrace()->traceEvent(TRACE_ERROR,
				     "Invalid '%s' value specified for -v: ignored",
				     optarg);
      } else {
	if(lvl < 0) lvl = 0;
	ntop->getTrace()->set_trace_level((u_int8_t)lvl);
      }
    }
    break;

  case 'F':
    if((strncmp(optarg, "es", 2) == 0) && (strlen(optarg) > 3)) {
      char *elastic_index_type = NULL, *elastic_index_name = NULL, *tmp = NULL,
	*elastic_url = NULL, *elastic_user = NULL, *elastic_pwd = NULL;
      /* es;<index type>;<index name>;<es URL>;<es pwd> */

      if((elastic_index_type = strtok_r(&optarg[3], ";", &tmp)) != NULL) {
	if((elastic_index_name = strtok_r(NULL, ";", &tmp)) != NULL) {
	  if((elastic_url = strtok_r(NULL, ";", &tmp)) != NULL) {
	    if((elastic_user = strtok_r(NULL, ";", &tmp)) == NULL)
	      elastic_pwd = (char*)"";
	    else {
	      char *double_col = strchr(elastic_user, ':');

	      if(double_col)
		elastic_pwd = &double_col[1], double_col[0] = '\0';
	      else
		elastic_pwd = (char*)"";
	    }
	  }
	}
      }

      if(elastic_index_type
	 && elastic_index_name
	 && elastic_url) {
	free(es_type), free(es_index), free(es_url), free(es_user), free(es_pwd);

	es_type  = strdup(elastic_index_type);
	es_index = strdup(elastic_index_name);
	es_url   = strdup(elastic_url);
	es_user  = strdup(elastic_user ? elastic_user : "");
	es_pwd   = strdup(elastic_pwd ? elastic_pwd : "");
	ntop->getTrace()->traceEvent(TRACE_NORMAL, "Using ElasticSearch for data dump [%s][%s][%s]",
				     es_type, es_index, es_url);
	dump_flows_on_es = true;
      } else {
	ntop->getTrace()->traceEvent(TRACE_WARNING,
				     "Discarding -F: invalid format for es");
	ntop->getTrace()->traceEvent(TRACE_WARNING,
				     "Format: -F es;<index type>;<index name>;<es URL>;<user>:<pwd>");
      }
    } else if(!strncmp(optarg, "mysql", 5)) {
      if(!strncmp(optarg, "mysql-nprobe", 12))
	read_flows_from_mysql = true;
      else
	dump_flows_on_mysql = true;

      /* mysql;<host[@port]|unix socket>;<dbname>;<table name>;<user>;<pw> */
      optarg = Utils::tokenizer(strchr(optarg, ';') + 1, ';', &mysql_host);
      optarg = Utils::tokenizer(optarg, ';', &mysql_dbname);
      optarg = Utils::tokenizer(optarg, ';', &mysql_tablename);
      optarg = Utils::tokenizer(optarg, ';', &mysql_user);
      mysql_pw = strdup(optarg ? optarg : "");

      if(mysql_host && mysql_user) {
	if((mysql_dbname == NULL) || (mysql_dbname[0] == '\0'))       mysql_dbname  = strdup("ntopng");
	if((mysql_tablename == NULL)
	   || (mysql_tablename[0] == '\0')
	   || dump_flows_on_mysql /*forcefully defaults the table name*/) {
	  if(mysql_tablename) free(mysql_tablename);
	  mysql_tablename  = strdup("flows");
	}
	if((mysql_pw == NULL) || (mysql_pw[0] == '\0')) mysql_pw  = strdup("");


      }  else
	ntop->getTrace()->traceEvent(TRACE_WARNING, "Invalid format for -F mysql;....");     
    } else if(!strncmp(optarg, "logstash", strlen("logstash"))) {
      /* logstash;<host[@port]; */
      ntop->getTrace()->traceEvent(TRACE_INFO, "Trying to get host for logstash");  
      optarg = Utils::tokenizer(&optarg[9],';',&ls_host);
      optarg = Utils::tokenizer(optarg,';',&ls_proto);
      ls_port = strdup(optarg ? optarg : NULL);

      if(ls_host && ls_proto && ls_port){
	ntop->getTrace()->traceEvent(TRACE_INFO," Dumping flows to logstash.");
	dump_flows_on_ls = true;
      } else {
	ntop->getTrace()->traceEvent(TRACE_WARNING, "Invalid format for -F logstash;....");
      }
    } else
      ntop->getTrace()->traceEvent(TRACE_WARNING, "Discarding -F %s: value out of range", optarg);
    break;

#ifndef WIN32
  case 'G':
    pid_path = strdup(optarg);
    break;
#endif

  case 'H':
    has_cmdl_disable_alerts = true;
    set_alerts_status(false);
    break;

  case 'I':
    export_endpoint = strdup(optarg);
    break;

  case 'U':
    free(user);
    user = strdup(optarg);
    break;

  case 'V':
    printf("v.%s\t[%s%s build]\n", PACKAGE_VERSION,
#ifdef NTOPNG_PRO
	   "Enterprise/Professional"
#else
	   "Community"
#endif
	   ,
#ifdef NTOPNG_EMBEDDED_EDITION
	   "/Embedded"
#else
	   ""
#endif
	   );
    printf("GIT rev:\t%s\n", NTOPNG_GIT_RELEASE);
#ifdef NTOPNG_PRO
    printf("Pro rev:\t%s\n", NTOPNG_PRO_GIT_RELEASE);
    printf("Built on:\t%s\n", PACKAGE_OS);

    printf("System Id:\t%s\n", ntop->getPro()->get_system_id());

    ntop->getTrace()->set_trace_level((u_int8_t)0);
    ntop->registerPrefs(this, true);
    ntop->getPro()->init_license();
    printf("Platform:\t%s\n", PACKAGE_MACHINE);
    printf("Edition:\t%s\n",      ntop->getPro()->get_edition());
    printf("License Type:\t%s\n", ntop->getPro()->get_license_type());

    if(ntop->getPro()->demo_ends_at())
      printf("Validity:\t%s\n", ntop->getPro()->get_demo_expiration(buf, sizeof(buf)));
    else
      printf("Maintenance:\t%s\n", ntop->getPro()->get_maintenance_expiration(buf, sizeof(buf)));

    if(ntop->getPro()->get_license()[0] != '\0')
      printf("License:\t%s\n",      ntop->getPro()->get_license());
#endif
    exit(0);
    break;

  case 'X':
    max_num_flows = max_val(atoi(optarg), 1024);
    break;

  case 210:
    nDPIhelp();
    break;

  case 211:
    online_license_check = true;
    break;

  case 212:
    if(!strcmp(optarg, "ixia"))
      enable_ixia_timestamps = true;
    else if((!strcmp(optarg, "vss")) || (!strcmp(optarg, "apcon")))
      enable_vss_apcon_timestamps = true;
    else
      ntop->getTrace()->traceEvent(TRACE_WARNING,
				   "Unknown --hw-timestamp-mode mode, it has been ignored\n");
    break;

  case 213:
    shutdown_when_done = true;
    break;

  case 214:
    simulate_vlans = true;
    break;

  case 215:
    zmq_encryption_pwd = strdup(optarg);
    break;
    
  case 216:
    enable_user_scripts = true;
    ntop->getTrace()->traceEvent(TRACE_NORMAL, "User scripts enabled");
    break;

#ifdef NTOPNG_PRO
  case 252:
    /* Disable tracing messages */
    ntop->getTrace()->set_trace_level(0);
    ntop->registerPrefs(this, true);
    ntop->getPro()->check_maintenance_duration();
    exit(0);
    break;

  case 253:
    /* Disable tracing messages */
    ntop->getTrace()->set_trace_level(0);
    ntop->registerPrefs(this, true);
    ntop->getPro()->check_license_validity();
    exit(0);
    break;

  case 254:
    ntop->getPro()->do_force_community_edition();
    break;
#endif

  default:
    ntop->getTrace()->traceEvent(TRACE_WARNING, "Unknown option -%c: Ignored.", (char)optkey);
    return(-1);
  }

  return(0);
}

/* ******************************************* */

int Prefs::checkOptions() {
#ifndef WIN32
  if(daemonize)
#endif
    {
      char path[MAX_PATH];

      ntop_mkdir(data_dir, 0777);
      ntop_mkdir(ntop->get_working_dir(), 0777);
      snprintf(path, sizeof(path), "%s/ntopng.log", ntop->get_working_dir() /* "C:\\Windows\\Temp" */);
      ntop->fixPath(path);
      ntop->registerLogFile(path);
      ntop->rotateLogs(true /* Force rotation to start clean */);
    }

  if(install_dir)
    ntop->set_install_dir(install_dir);

  free(data_dir);
  data_dir = strdup(ntop->get_install_dir());

  if(!prefs_dir)
    prefs_dir = strdup(ntop->get_working_dir());

  docs_dir      = ntop->getValidPath(docs_dir);
  scripts_dir   = ntop->getValidPath(scripts_dir);
  callbacks_dir = ntop->getValidPath(callbacks_dir);

  if(!data_dir)         { ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to locate data dir");      return(-1); }
  if(!docs_dir[0])      { ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to locate docs dir");      return(-1); }
  if(!scripts_dir[0])   { ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to locate scripts dir");   return(-1); }
  if(!callbacks_dir[0]) { ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to locate callbacks dir"); return(-1); }
  if(!prefs_dir[0])     { ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to locate prefs dir");     return(-1); }

  ntop->removeTrailingSlash(docs_dir);
  ntop->removeTrailingSlash(scripts_dir);
  ntop->removeTrailingSlash(callbacks_dir);
  ntop->removeTrailingSlash(prefs_dir);

  if(http_binding_address == NULL)
    http_binding_address = strdup((char*)CONST_ANY_ADDRESS);
  if(https_binding_address == NULL)
    https_binding_address = strdup((char*)CONST_ANY_ADDRESS);
  return(0);
}

/* ******************************************* */

int Prefs::loadFromCLI(int argc, char *argv[]) {
  u_char c;

  while((c = getopt_long(argc, argv,
			 "k:eg:hi:w:r:sg:m:n:p:qd:t:x:1:2:3:4:l:uv:A:B:CD:E:F:N:G:HI:O:Q:S:TU:X:W:VZ:",
			 long_options, NULL)) != '?') {
    if(c == 255) break;
    setOption(c, optarg);
  }

  if((http_port == 0) && (https_port == 0)) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "Both HTTP and HTTPS ports are disabled: quitting");
    exit(0);
  }

  return(checkOptions());
}

/* ******************************************* */

int Prefs::loadFromFile(const char *path) {
  char buffer[4096], *line, *key, *value;
  FILE *fd;
  const struct option *opt;

  config_file_path = strdup(path);

  fd = fopen(config_file_path, "r");

  if(fd == NULL) {
    ntop->getTrace()->traceEvent(TRACE_WARNING, "Config file %s not found", config_file_path);
    return(-1);
  }

  while(true) {
    if(!(line = fgets(buffer, sizeof(buffer), fd)))
      break;

    line = Utils::trim(line);

    if(strlen(line) < 1 || line[0] == '#')
      continue;

    key = line;
    key = Utils::trim(key);

    value = strrchr(line, '=');

    /* Fallback to space */
    if(value == NULL) value = strchr(line, ' ');

    if(value == NULL)
      value = &line[strlen(line)]; /* empty */
    else
      value[0] = 0, value = &value[1];
    value = Utils::trim(value);

    if(strlen(key) > 2) key = &key[2];
    else key = &key[1];

    opt = long_options;
    while (opt->name != NULL) {
      if((strcmp(opt->name, key) == 0)
	 || ((key[1] == '\0') && (opt->val == key[0]))) {
        setOption(opt->val, value);
        break;
      }
      opt++;
    }
  }

  fclose(fd);

  return(checkOptions());
}

/* ******************************************* */

void Prefs::add_network_interface(char *name, char *description) {
  int id = Utils::ifname2id(name);

  if(id < (MAX_NUM_INTERFACES-1)) {
    ifNames[id].name = strdup(!strncmp(name, "-", 1) ? "stdin" : name);
    ifNames[id].description = strdup(description ? description : name);
    num_interfaces++;
  } else {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "Too many interfaces (%d): discarded %s", id, name);
    ntop->getTrace()->traceEvent(TRACE_ERROR, "Hint: reset redis (redis-cli flushall) and then start ntopng again");
  }
}

/* ******************************************* */

void Prefs::add_default_interfaces() {
  NetworkInterface *dummy = new NetworkInterface("dummy");
  dummy->addAllAvailableInterfaces();
  delete dummy;
};

/* *************************************** */

void Prefs::lua(lua_State* vm) {
#ifdef NTOPNG_PRO
  char HTTP_stats_base_dir[MAX_PATH*2];
#endif

  lua_newtable(vm);

  lua_push_bool_table_entry(vm, "is_dns_resolution_enabled_for_all_hosts", resolve_all_host_ip);
  lua_push_bool_table_entry(vm, "is_dns_resolution_enabled", enable_dns_resolution);
  lua_push_bool_table_entry(vm, "is_categorization_enabled", flashstart ? true : false);
  lua_push_bool_table_entry(vm, "is_httpbl_enabled", is_httpbl_enabled());
  lua_push_bool_table_entry(vm, "is_autologout_enabled", enable_auto_logout);
  lua_push_int_table_entry(vm, "http_port", http_port);

  lua_push_int_table_entry(vm, "max_num_hosts", max_num_hosts);
  lua_push_int_table_entry(vm, "max_num_flows", max_num_flows);
  lua_push_bool_table_entry(vm, "is_dump_flows_enabled", dump_flows_on_es || dump_flows_on_mysql || dump_flows_on_ls);
  lua_push_bool_table_entry(vm, "is_dump_flows_to_mysql_enabled", dump_flows_on_mysql || read_flows_from_mysql);
  lua_push_bool_table_entry(vm, "is_flow_aggregation_enabled", is_flow_aggregation_enabled());
  if(is_flow_aggregation_enabled())
    lua_push_int_table_entry(vm, "flow_aggregation_frequency", flow_aggregation_frequency());

  if(mysql_dbname) lua_push_str_table_entry(vm, "mysql_dbname", mysql_dbname);
  lua_push_bool_table_entry(vm, "is_dump_flows_to_es_enabled",    dump_flows_on_es);
  lua_push_bool_table_entry(vm, "is_dump_flows_to_ls_enabled", dump_flows_on_ls);

  lua_push_bool_table_entry(vm, "are_user_scripts_enabled", enable_user_scripts);
  lua_push_int_table_entry(vm, "dump_hosts", dump_hosts_to_db);

  lua_push_int_table_entry(vm, "http.port", get_http_port());
  lua_push_int_table_entry(vm, "http.alt_port", get_alt_http_port());

  lua_push_str_table_entry(vm, "instance_name", instance_name ? instance_name : (char*)"");

  /* Sticky hosts preferences */
  if(sticky_hosts != location_none) {
    char *location_string = NULL;
    
    if(sticky_hosts == location_all) location_string = (char*)"all";
    else if(sticky_hosts == location_local_only) location_string = (char*)"local";
    else if(sticky_hosts == location_remote_only) location_string = (char*)"remote";
    if(location_string) lua_push_str_table_entry(vm, "sticky_hosts", location_string);
  }
  
  /* Command line options */
  lua_push_bool_table_entry(vm, "has_cmdl_trace_lvl", has_cmdl_trace_lvl);
  lua_push_bool_table_entry(vm, "has_cmdl_disable_alerts", has_cmdl_disable_alerts);

#if defined(NTOPNG_PRO) && !defined(WIN32)
  if(ntop->getNagios()) ntop->getNagios()->lua(vm);
#endif

#ifdef NTOPNG_PRO
  memset(HTTP_stats_base_dir, '\0', MAX_PATH);
  strncat(HTTP_stats_base_dir, (const char*)ntop->get_working_dir(), MAX_PATH);
  strncat(HTTP_stats_base_dir, "/httpstats/", MAX_PATH);
  lua_push_str_table_entry(vm, "http_stats_base_dir", HTTP_stats_base_dir);
#endif

  RuntimePrefs::lua(vm);
}

/* *************************************** */

void Prefs::refreshHostsAlertsPrefs() {
  char rsp[32];

  if(ntop->getRedis()->hashGet((char*)CONST_RUNTIME_PREFS_HOSTS_ALERTS_CONFIG,
          (char*)CONST_HOST_FLOW_ATTACKER_ALERT_THRESHOLD_KEY, rsp, sizeof(rsp)) == 0)
    attacker_max_num_flows_per_sec = atol(rsp);
  else
    attacker_max_num_flows_per_sec = CONST_MAX_NEW_FLOWS_SECOND;

  if(ntop->getRedis()->hashGet((char*)CONST_RUNTIME_PREFS_HOSTS_ALERTS_CONFIG,
          (char*)CONST_HOST_FLOW_VICTIM_ALERT_THRESHOLD_KEY, rsp, sizeof(rsp)) == 0)
    victim_max_num_flows_per_sec = atol(rsp);
  else
    victim_max_num_flows_per_sec = CONST_MAX_NEW_FLOWS_SECOND;

  if(ntop->getRedis()->hashGet((char*)CONST_RUNTIME_PREFS_HOSTS_ALERTS_CONFIG,
          (char*)CONST_HOST_SYN_ATTACKER_ALERT_THRESHOLD_KEY, rsp, sizeof(rsp)) == 0)
    attacker_max_num_syn_per_sec = atol(rsp);
  else
    attacker_max_num_syn_per_sec = CONST_MAX_NUM_SYN_PER_SECOND;

  if(ntop->getRedis()->hashGet((char*)CONST_RUNTIME_PREFS_HOSTS_ALERTS_CONFIG,
          (char*)CONST_HOST_SYN_VICTIM_ALERT_THRESHOLD_KEY, rsp, sizeof(rsp)) == 0)
    victim_max_num_syn_per_sec = atol(rsp);
  else
    victim_max_num_syn_per_sec = CONST_MAX_NUM_SYN_PER_SECOND;
}

/* *************************************** */

void Prefs::refreshLanWanInterfaces() {
  char rsp[32];

  if(lan_interface) free(lan_interface);
  if(wan_interface) free(wan_interface);

  if(ntop->getRedis()->get((char*)CONST_RUNTIME_PREFS_LAN_INTERFACE,
      rsp, sizeof(rsp)) == 0)
    lan_interface = strdup(rsp);
  else
    lan_interface = NULL;

  if(ntop->getRedis()->get((char*)CONST_RUNTIME_PREFS_WAN_INTERFACE,
      rsp, sizeof(rsp)) == 0)
    wan_interface = strdup(rsp);
  else
    wan_interface = NULL;
}

/* *************************************** */

void Prefs::refreshMacNdpiStatsPrefs() {
  char rsp[32];

  if((ntop->getRedis()->get((char*)CONST_RUNTIME_PREFS_ENABLE_MAC_NDPI_STATS,
      rsp, sizeof(rsp)) == 0) && (rsp[0] == '1'))
    enable_mac_ndpi_stats = true;
  else
    enable_mac_ndpi_stats = false;
}

/* *************************************** */

void Prefs::registerNetworkInterfaces() {
  for(int i=0; i<num_deferred_interfaces_to_register; i++) {
    if(deferred_interfaces_to_register[i] != NULL) {
      add_network_interface(deferred_interfaces_to_register[i], NULL);
      free(deferred_interfaces_to_register[i]);
    }
  }
}

/* *************************************** */

bool Prefs::is_pro_edition() {
  return
#ifdef NTOPNG_PRO
    ntop->getPro()->has_valid_license()
#else
  false
#endif
    ;
}

/* *************************************** */

bool Prefs::is_enterprise_edition() {
  return
#ifdef NTOPNG_PRO
    ntop->getPro()->has_valid_enterprise_license()
#else
  false
#endif
    ;
}

/* *************************************** */

time_t Prefs::pro_edition_demo_ends_at() {
  return
#ifdef NTOPNG_PRO
    ntop->getPro()->demo_ends_at()
#else
    0
#endif
    ;
}
