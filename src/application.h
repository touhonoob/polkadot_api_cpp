typedef struct ProtocolParameters {
    Hasher FreeBalanceHasher;
    string FreeBalancePrefix;
} ProtocolParameters;

class ApplicationException : public exception {
    string _msg;

public:
    ApplicationException(string msg) { _msg = msg; };
    virtual char const *what() const noexcept { return _msg.c_str(); }
};

class CPolkaApi : public IApplication, IWebSocketMessageObserver {
private:
    ILogger *_logger;
    IJsonRpc *_jsonRpc;

    unique_ptr<SystemInfo> createSystemInfo(Json jsonObject);
    unique_ptr<BlockHash> createBlockHash(Json jsonObject);
    unique_ptr<RuntimeVersion> createRuntimeVersion(Json jsonObject);
    unique_ptr<Metadata> createMetadata(Json jsonObject);
    template <typename T, unique_ptr<T> (CPolkaApi::*F)(Json)> unique_ptr<T> deserialize(Json jsonObject);
    template <typename T> T fromHex(string hexStr, bool bigEndian = true);
    Hasher getFuncHasher(unique_ptr<Metadata> &meta, const string &moduleName, const string &funcName);

    ProtocolParameters _protocolPrm;

    // Implements IWebSocketMessageObserver
    void handleWsMessage(const int subscriptionId, const Json &message);

    // Subscriber functors
    std::function<void(long long)> _blockNumberSubscriber;
    map<string, std::function<void(unsigned __int128)>> _balanceSubscribers;

    // Subscription IDs
    int _blockNumberSubscriptionId;
    map<string, int> _balanceSubscriptionIds;

public:
    CPolkaApi() = delete;
    CPolkaApi(ILogger *logger, IJsonRpc *jsonRpc);
    virtual ~CPolkaApi() override {}
    virtual int connect();
    virtual void disconnect();
    virtual unique_ptr<SystemInfo> getSystemInfo();
    virtual unique_ptr<BlockHash> getBlockHash(unique_ptr<GetBlockHashParams> params);
    virtual unique_ptr<Metadata> getMetadata(unique_ptr<GetMetadataParams> params);
    virtual unique_ptr<RuntimeVersion> getRuntimeVersion(unique_ptr<GetRuntimeVersionParams> params);

    virtual int subscribeBlockNumber(std::function<void(long long)> callback);
    virtual int unsubscribeBlockNumber();
    virtual int subscribeBalance(string address, std::function<void(unsigned __int128)> callback);
    virtual int unsubscribeBalance(string address);
};
