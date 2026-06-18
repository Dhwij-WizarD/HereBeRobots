#pragma once

#include <concepts>
#include <functional>
#include <string>
#include <diagnostics/Status.hpp>

namespace HBR::Communication
{

    template<typename T, typename MessageT>
    concept IsPublisher =
    requires(T publisher, const MessageT& message)
    {
        publisher.Publish(message);
    };

    template<typename T>
    using Callback = std::function<STATUS(const T&)>;

    template<typename RequestT, typename ResponseT>
    using ResponderCallback = std::function<ResponseT(const RequestT&)>;

    template<typename T, typename MessageT>
    concept IsSubscriber =
    requires(T subscriber, Callback<MessageT> callback)
    {
        subscriber.Subscribe(callback);
    };

    template<typename T, typename RequestT, typename ResponseT>
    concept IsRequester =
    requires(T requester, const RequestT& request)
    {
        { requester.Request(request) } -> std::same_as<ResponseT>;
    };

    template<typename T, typename RequestT, typename ResponseT>
    concept IsResponder =
    requires(T responder, const RequestT& request)
    {
        { responder.Respond(request) } -> std::same_as<ResponseT>;
    };

    template<typename T, typename StreamT>
    concept IsStreamer =
    requires(T streamer, const StreamT& data)
    {
        streamer.StartStream();
        streamer.StopStream();
        streamer.Send(data);
    };

    template<typename T, typename StreamT>
    concept IsAudience =
    requires(T audience, Callback<StreamT> callback)
    {
        audience.JoinStream(callback);
        audience.LeaveStream();
    };

    // Manages a dynamic collection of publishers and subscriptions.
    // Not parameterized by message type — channels of any type can be added.
    // Proxy type int is used in requires to verify the template method shape.
    template<typename T>
    concept IsMessenger =
    requires(T messenger, const std::string& name, Callback<int> cb)
    {
        messenger.template AddPublisher<int>(name);
        messenger.template AddSubscription<int>(name, cb);
        messenger.DeletePublisher(name);
        messenger.DeleteSubscription(name);
    };

    template<typename T>
    concept IsCommunicator =
        IsMessenger<T> &&
        requires(T comm, const std::string& name, ResponderCallback<int, int> rcb)
        {
            comm.template AddRequester<int, int>(name);
            comm.template AddResponder<int, int>(name, rcb);
            comm.DeleteRequester(name);
            comm.DeleteResponder(name);
        };
    
    template<typename T>
    class Communicator
    {
        public:
            STATUS Init();
            STATUS Deinit();
            template<typename REQ, typename RES>
            AddRequester<REQ, RES>(const std::string& name);
            template<typename REQ, typename RES>
            AddResponder<REQ, RES>(const std::string& name, ResponderCallback<REQ, RES> rcb);
            DeleteRequester(const std::string& name);
            DeleteResponder(const std::string& name);
            template<typename MSG>
            AddPublisher<MSG>(const std::string& name);
            AddSubscription<MSG>(const std::string& name, Callback<MSG> cb);
            DeletePublisher(const std::string& name);
            DeleteSubscription(const std::string& name);
        protected:
        private:
    }

} // namespace HBR::Communication
