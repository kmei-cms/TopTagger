#include "TopTagger/TopTagger/include/TTMTensorflow.h"

#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"
#include "TopTagger/TopTagger/include/TopObject.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"
#include "TopTagger/CfgParser/include/TTException.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
void TTMTensorflow::getParameters(const cfg::CfgDocument* cfgDoc, const std::string& localContextName)
{
#ifdef DOTENSORFLOW
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt(localContextName);

    discriminator_ = cfgDoc->get("discCut",       localCxt, -999.9);
    discOffset_    = cfgDoc->get("discOffset",    localCxt, 999.9);
    discSlope_     = cfgDoc->get("discSlope",     localCxt, 0.0);
    modelFile_     = cfgDoc->get("modelFile",     localCxt, "");
    inputOp_       = cfgDoc->get("inputOp",       localCxt, "x");
    outputOp_      = cfgDoc->get("outputOp",      localCxt, "y");
    NConstituents_ = cfgDoc->get("NConstituents", localCxt, 3);

    csvThreshold_  = cfgDoc->get("csvThreshold", localCxt, -999.9);
    bEtaCut_       = cfgDoc->get("bEtaCut",      localCxt, -999.9);
    maxNbInTop_    = cfgDoc->get("maxNbInTop",   localCxt, -1);

    int iVar = 0;
    bool keepLooping;
    do
    {
        keepLooping = false;

        //Get variable name
        std::string varName = cfgDoc->get("mvaVar", iVar, localCxt, "");

        //if it is a non empty string save in vector
        if(varName.size() > 0)
        {
            keepLooping = true;

            vars_.push_back(varName);
        }
        ++iVar;
    }
    while(keepLooping);

    //Variable to hold tensorflow status
    TF_Status* status = TF_NewStatus();

    //get the grafdef from the file
    TF_Buffer* graph_def = read_file(modelFile_);

    // Import graph_def into graph
    TF_Graph* graph = TF_NewGraph();
    TF_ImportGraphDefOptions* graph_opts = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, graph_def, graph_opts, status);
    TF_DeleteImportGraphDefOptions(graph_opts);
    TF_DeleteBuffer(graph_def);

    if (TF_GetCode(status) != TF_OK) 
    {
        THROW_TTEXCEPTION("ERROR: Unable to import graph: " + std::string(TF_Message(status)));
    }

    //Create tensorflow session from imported graph
    TF_SessionOptions* sess_opts = TF_NewSessionOptions();
    uint8_t config[] = {0x10, 0x01};
    TF_SetConfig(sess_opts, static_cast<void*>(config), 2, status);
    session_ = TF_NewSession(graph, sess_opts, status);
    TF_DeleteSessionOptions(sess_opts);

    if (TF_GetCode(status) != TF_OK) 
    {
        THROW_TTEXCEPTION("ERROR: Unable to create tf session: " + std::string(TF_Message(status)));
    }

    TF_Operation* op_x = TF_GraphOperationByName(graph, inputOp_.c_str());
    TF_Operation* op_y = TF_GraphOperationByName(graph, outputOp_.c_str());

    //Clean up graph
    TF_DeleteGraph(graph);

    if(op_x == nullptr)
    {
        THROW_TTEXCEPTION("Input operation \"" + inputOp_ + "\" not found in graph");
    }    

    if(op_y == nullptr)
    {
        THROW_TTEXCEPTION("Output operation \"" + outputOp_ + "\" not found in graph");
    }

    inputs_ .emplace_back(TF_Output({op_x, 0}));
    outputs_.emplace_back(TF_Output({op_y, 0}));
    targets_.emplace_back(op_y);

    //Construct tensorflow input tensor
    const int elemSize = sizeof(float);
    std::vector<int64_t> dims = {1, static_cast<int64_t>(vars_.size())};
    int nelem = 1;
    for(const auto dimLen : dims) nelem += dimLen;
    TF_Tensor* input_values_0 =  TF_AllocateTensor(TF_FLOAT, dims.data(), dims.size(), elemSize*nelem);

    input_values_ = {  input_values_0 };

    TF_DeleteStatus(status);

    //load variables
    if(NConstituents_ == 1)
    {
        varCalculator_.reset(new ttUtility::BDTMonojetInputCalculator());
    }
    else if(NConstituents_ == 2)
    {
        varCalculator_.reset(new ttUtility::BDTDijetInputCalculator());
    }
    else if(NConstituents_ == 3)
    {
        varCalculator_.reset(new ttUtility::TrijetInputCalculator());
    }
    //map variables
    varCalculator_->mapVars(vars_, static_cast<float*>(TF_TensorData(input_values_0)));

#else
    THROW_TTEXCEPTION("ERROR: TopTagger not compiled with Tensorflow support!!!");
#endif
}

void TTMTensorflow::run(TopTaggerResults& ttResults)
{
#ifdef DOTENSORFLOW
    //Get the list of top candidates as generated by the clustering algo
    std::vector<TopObject>& topCandidates = ttResults.getTopCandidates();
    //Get the list of final tops into which we will stick candidates
    std::vector<TopObject*>& tops = ttResults.getTops();

    //tensorflow status variable
    TF_Status* status = TF_NewStatus();

    //Create place to store the output vectors 
    std::vector<TF_Tensor*>    output_values(1);

    for(auto& topCand : topCandidates)
    {
        //Prepare data from top candidate (this code is shared with training tuple producer)
        if(varCalculator_->calculateVars(topCand))
        {
            //predict value
            TF_SessionRun(session_,
                          // RunOptions
                          nullptr,
                          // Input tensors
                          inputs_.data(), input_values_.data(), inputs_.size(),
                          // Output tensors
                          outputs_.data(), output_values.data(), outputs_.size(),
                          // Target operations
                          targets_.data(), targets_.size(),
                          // RunMetadata
                          nullptr,
                          // Output status
                          status);

            if (TF_GetCode(status) != TF_OK)
            {
                THROW_TTEXCEPTION("ERROR: Unable to run graph: " + std::string(TF_Message(status)));
            }

            //Get output discriminator 
            auto discriminators = static_cast<float*>(TF_TensorData(output_values[0]));            
            double discriminator = static_cast<double>(discriminators[0]);
            topCand.setDiscriminator(discriminator);
            for(auto tensor : output_values) TF_DeleteTensor(tensor);
            
            //Check number of b-tagged jets in the top
            bool passBrequirements = maxNbInTop_ < 0 || topCand.getNBConstituents(csvThreshold_, bEtaCut_) <= maxNbInTop_;

            //place in final top list if it passes the threshold
            if(discriminator > std::min(discriminator_, discOffset_ + topCand.p().Pt()*discSlope_) && passBrequirements)
            {
                tops.push_back(&topCand);
            }
        }
        break;
    }

    TF_DeleteStatus(status);
#endif
}

#ifdef DOTENSORFLOW

TTMTensorflow::~TTMTensorflow()
{
    //tensorflow status variable
    TF_Status* status = TF_NewStatus();

    for(auto tensor : input_values_)  TF_DeleteTensor(tensor);
    
    TF_DeleteSession(session_, status);

    if (TF_GetCode(status) != TF_OK)
    {
        //THROW_TTEXCEPTION("ERROR: Unable to delete tf session: " + std::string(TF_Message(status)));
    }

    TF_DeleteStatus(status);
}

void free_buffer(void* data, size_t length) 
{
    free(data);
}

TF_Buffer* TTMTensorflow::read_file(const std::string& file) 
{
    FILE *f = fopen(file.c_str(), "rb");

    if(f == nullptr)
    {
        THROW_TTEXCEPTION("File not found: \"" + file + "\"");
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    void* data = malloc(fsize);
    fread(data, fsize, 1, f);
    fclose(f);

    TF_Buffer* buf = TF_NewBuffer();
    buf->data = data;
    buf->length = fsize;
    buf->data_deallocator = free_buffer;
    return buf;
}
#endif
