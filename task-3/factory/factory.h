# pragma once

#include <iostream>
#include <map>
#include "functions.h"
#include <vector>
#include <memory>

class TFactory {
    class TImpl;

    std::unique_ptr<const TImpl> Impl;
public:
    TFactory();

    ~TFactory();

    template<typename T>
    std::shared_ptr<TFunction> CreateObject(const std::string &name, T &&params) const;

    [[nodiscard]] std::shared_ptr<TFunction> CreateObject(const std::string &name, std::initializer_list<int> params) const;

    [[nodiscard]] std::shared_ptr<TFunction> CreateObject(const std::string &name) const;

    [[nodiscard]] std::vector<std::string> GetAvailableFunctions() const;
};

class TFactory::TImpl {

    class ICreator {
    public:
        virtual ~ICreator() = default;

        virtual std::shared_ptr<TFunction> Create(void *params) const = 0;

        [[nodiscard]] virtual std::shared_ptr<TFunction> Create() const = 0;
    };

    using TCreatorPtr = std::shared_ptr<ICreator>;
    using TRegisteredCreators = std::map<std::string, TCreatorPtr>;
    TRegisteredCreators RegisteredCreators;

public:

    template<class TCurrentObject, typename TParam>
    class TCreator : public ICreator {

        std::shared_ptr<TFunction> Create(void *params) const override {
            return std::make_shared<TCurrentObject>(*(static_cast<TParam *>(params)));
        }

        [[nodiscard]] std::shared_ptr<TFunction> Create() const override {
            return std::make_shared<TCurrentObject>();
        }
    };

    TImpl() { RegisterAll(); }

    template<typename T, typename P>
    void RegisterCreator(const std::string &name) {
        RegisteredCreators[name] = std::make_shared<TCreator<T, P>>();
    }

    void RegisterAll() {
        RegisterCreator<TIdent, int>("ident");
        RegisterCreator<TConst, int>("const");
        RegisterCreator<TPower, int>("power");
        RegisterCreator<TExp, int>("exp");
        RegisterCreator<TPolynomial, std::initializer_list<int>>("polynomial");
    }

    template<typename T>
    std::shared_ptr<TFunction> CreateObject(const std::string &n, T &&params) const {
        auto creator = RegisteredCreators.find(n);
        if (creator == RegisteredCreators.end()) {
            return nullptr;
        }
        return creator->second->Create(std::addressof(params));
    }

    [[nodiscard]] std::shared_ptr<TFunction> CreateObject(const std::string &n) const {
        auto creator = RegisteredCreators.find(n);
        if (creator == RegisteredCreators.end()) {
            return nullptr;
        }
        return creator->second->Create();
    }

    [[nodiscard]] std::vector<std::string> GetAvailableFunctions() const {
        std::vector<std::string> result;
        for (const auto &creatorPair : RegisteredCreators) {
            result.push_back(creatorPair.first);
        }
        return result;
    }

};

TFactory::TFactory() : Impl(std::make_unique<TFactory::TImpl>()) {}

TFactory::~TFactory() = default;

std::vector<std::string> TFactory::GetAvailableFunctions() const {
    return Impl->GetAvailableFunctions();
}

template<typename T>
std::shared_ptr<TFunction> TFactory::CreateObject(const std::string &name, T &&params) const {
    return Impl->CreateObject(name, std::forward<T>(params));
}

std::shared_ptr<TFunction> TFactory::CreateObject(const std::string &name, std::initializer_list<int> params) const {
    return Impl->CreateObject(name, params);
}

std::shared_ptr<TFunction> TFactory::CreateObject(const std::string &name) const {
    return Impl->CreateObject(name);
}