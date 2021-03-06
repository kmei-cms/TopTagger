#ifndef TTMAK8TOPFILTER_H
#define TTMAK8TOPFILTER_H

#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TTMFilterBase.h"

class TopTaggerResults;
class Constituent;

class TTMAK8TopFilter : public TTModule
{
private:

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMAK8TopFilter);

#endif
