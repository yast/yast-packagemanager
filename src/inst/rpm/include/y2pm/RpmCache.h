#ifndef RpmCache_h
#define RpmCache_h
struct rpmCache {
    std::list<std::string> _description;
    std::string _buildhost;
    Date _installtime;
    std::string _distribution;
    std::string _vendor;
    std::string _license;
    std::string _packager;
    std::string _url;
    std::string _os;
    std::string _sourcerpm;

    rpmCache() : _installtime(0) {}
};
#endif // RpmCache_h
