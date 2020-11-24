#include <iostream>
#include <memory>

class Any {
    struct HolderBase
    {

        virtual const std::type_info& GetType() const = 0;
        virtual ~HolderBase() = default;
    };

    template<typename T> struct Holder : HolderBase
    {
        T value;
        explicit Holder(const T& t) : value(t) { }
        const std::type_info& GetType() const override{
            return typeid(value);
        }
    };

    HolderBase* holder;
    
public:

    Any() = default;

    template <typename T>
    explicit Any(const T& v) : holder(new Holder<T>(v)) {}

    template<typename U>
    U get(){
        if(typeid(U) != holder->GetType()) {
            throw std::runtime_error("error");
        }
        return static_cast<Holder<U>*> (holder)->value;
    }

    ~Any() {
        delete holder;
    }
};

int main() {
    Any a(5);
    a.get<int>(); // 5
    //a.get<std::string>(); //error
    return 0;
}
