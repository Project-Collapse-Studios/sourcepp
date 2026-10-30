#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <sourcepp/StringReference.h>

#include <BufferStream.h>
#include <sourcepp/parser/Text.h>
#include <sourcepp/String.h>

#include <sourcepp/math/Vec.h>

#include <format>
#include <sourcepp/FS.h>

/// Defines conditions that allow us to compare the keys
#define RequiresKeyComparison ((std::convertible_to<K, std::string_view> && std::convertible_to<KeyType, std::string_view>) || std::equality_comparable_with<K, KeyType>)
/// Defines conditions that allow us to convert from V type to the type that's defined as the KeyType in KV1
#define RequiresKeyConversion ((std::convertible_to<K, std::string_view> && std::same_as<KeyType, strref>) || std::convertible_to<K, KeyType>)
/// Class uses string reference
#define UsesSTRREF (std::same_as<KeyType, strref> || std::same_as<ValueType, strref>)

namespace kvpp {

template <typename T>
struct is_math_vec : std::false_type {};

template<uint8_t S, mathpp::Arithmetic P>
struct is_math_vec<mathpp::Vec<S, P>> : std::true_type {};

template<typename V>
concept KV1ValueType = std::convertible_to<V, std::string>
                    || std::integral<V>
                    || std::floating_point<V>
					|| is_math_vec<V>::value;

/// Used for defining which types can we automatically convert to.
/// To prevent ambiguity, chars are not allowed, because then
/// converting to std::string makes an ambigous operator call.
/// std::string can construct from both operator std::string(), and operator char()
template<typename V>
concept KV1ValueConvertible = KV1ValueType<V> && !std::convertible_to<V, std::string>
	&& !std::same_as<std::decay_t<V>, char>
	&& !std::same_as<std::decay_t<V>, char8_t>
	&& !std::same_as<std::decay_t<V>, char16_t>
	&& !std::same_as<std::decay_t<V>, char32_t>
	&& !std::same_as<std::decay_t<V>, wchar_t>;


/// How the underlaying data is stored, this needs to be some sub-type of string.
template<typename V>
concept KV1UnderlayingValueType = !std::same_as<V, std::string_view> && (
								std::same_as<V, stringpp::string_reference> ||
								std::same_as<V, std::string>);

template<typename V>
concept KV1AllowedKeyTypes = !std::same_as<V, std::string_view> && (
							std::same_as<V, stringpp::string_reference> ||
							std::same_as<V, std::string> ||
							(std::is_array_v<V> && std::same_as<std::remove_extent_t<V>, const char>) ||
							std::same_as<V, char*> || std::same_as<V, const char*> );

/// Defines ways to compare the keys
template<KV1AllowedKeyTypes T0, KV1AllowedKeyTypes T1>
constexpr bool CompareKeys(const T0& key1, const T1& key2) {
	if constexpr (std::is_convertible_v<T0, std::string_view> &&
					std::is_convertible_v<T1, std::string_view>) {
		return sourcepp::string::iequals(key1, key2);
	} else {
		return key1 == key2;
	}
}

template<KV1AllowedKeyTypes KeyType = stringpp::string_reference, KV1UnderlayingValueType ValueType = stringpp::string_reference>
class KV1Element {
	using strref = stringpp::string_reference;

public:
	/// Get the key associated with the element
	[[nodiscard]] KeyType getKey() const {
		return this->key;
	}

	/// Set the key associated with the element
	template<KV1AllowedKeyTypes K = KeyType>
	void setKey(K key_) requires RequiresKeyConversion {
		this->key = key_;
	}

	/// Get the value associated with the element
	[[nodiscard]] ValueType getValue() const {
		return this->value;
	}

	/// Retrieve the string_reference manager, if we're based on str_refs
	[[nodiscard]] std::shared_ptr<stringpp::string_reference_manager> getSTRREFManager() const requires UsesSTRREF {
		return this->string_manager;
	}

	/// Get the value associated with the element as the given type
	template<KV1ValueType V, bool robust_parsing = false>
	[[nodiscard]] V getValue() const {

        if(this->isInvalid())
            return V{};

		if constexpr (std::convertible_to<V, std::string>) { // STRING

			return this->value;

		} else if constexpr (std::same_as<V, bool>) { // BOOL

			bool out;
			sourcepp::string::toArithmetic<bool, robust_parsing>(this->value, out);
			return out;

		} else if constexpr (std::is_integral_v<V>) { // INT

			V out = 0;
			if (this->value.length() == 10 && this->value.starts_with("0x") && sourcepp::parser::text::isNumber(this->value.substr(2))) {
				sourcepp::string::toInt(this->value.substr(2), out, 16);
			} else {
				sourcepp::string::toArithmetic<V, robust_parsing>(this->value, out);
			}
			return out;

		} else if constexpr (std::is_floating_point_v<V>) { // FLOAT

			float out = 0.f;
			sourcepp::string::toArithmetic<float, robust_parsing>(this->value, out);
			return out;

		}  else if constexpr (is_math_vec<V>::value) { // VECTOR

			auto splits = sourcepp::string::split(this->value, ' ');
			auto return_val = V{};
			for (uint8_t i = 0; i < return_val.S; ++i) {
				if (i >= splits.size()) {
					return_val.values[i] = 0;
				}
				sourcepp::string::toArithmetic<typename V::value_type, robust_parsing>(splits[i], return_val.values[i]);
			}
			return return_val;

		}
		return V{};
	}

	template<KV1ValueConvertible V>
	operator V() const {
		if constexpr( // Types that require robust parsing
			is_math_vec<V>::value // Vectors can be represented like "[x y z]" or just "x y z" etc.
		) {
			return this->getValue<V, true>();
		} else {
			return this->getValue<V, false>();
		}
	}

    template<KV1ValueConvertible V>
    operator std::vector<V>() const {
        std::vector<V> elements;
        for(const KV1Element & child : this->children) {
            if constexpr ( // Types that require robust parsing
                    is_math_vec<V>::value // Vectors can be represented like "[x y z]" or just "x y z" etc.
                    ) {
                elements.push_back(child.getValue<V, true>());
            } else {
                elements.push_back(child.getValue<V, false>());
            }
        }
        return elements;
    }

	operator std::string() const {
		return this->value;
	}

	/// Get n values (of the same type) from multiple children of the same key.
	template<KV1ValueType V, bool robust_parsing = false, KV1AllowedKeyTypes K = stringpp::string_reference>
	std::vector<V> getChildrenValues(K& childKey, uint32_t n = -1) const requires RequiresKeyComparison { // n = -1 will cause it to actually be the max value
		std::vector<V> result;
		auto child_count = this->getChildCount(childKey);
		n = n < child_count ? n : child_count;
		while (n--) {
			result.push_back(this->operator()(childKey, n).template getValue<V, robust_parsing>());
		}
		return result;
	}

	/// Set the value associated with the element
	template<KV1ValueType V>
	void setValue(V value_) {
		if constexpr (std::convertible_to<V, std::string>) {
			this->value = std::string{value_}; // Strref automatically accounts for this.
		} else if constexpr (std::same_as<V, bool>) {
			this->setValue(std::format("{:b}", value_));
		} else {
			this->setValue(std::format("{}", value_));
		}
		// TODO: Add vector support here!
	}


	/// Get the conditional associated with the element
	[[nodiscard]] const std::string& getConditional() const {
		return this->conditional;
	}

	/// Set the conditional associated with the element
	void setConditional(const std::string& conditional_) {
		this->conditional = conditional_;
	}

	/// Check if the element has one or more children with the given key
	template<KV1AllowedKeyTypes K>
	[[nodiscard]] bool hasChild(const K childKey) const requires RequiresKeyComparison {
		for (const auto& el : *this) {
			if (CompareKeys(el.key, childKey)) {
				return true;
			}
		}
		return false;
	}

	/// Add a new child element with the given key, with an optional value
	template<KV1AllowedKeyTypes K, KV1ValueType V = ValueType>
	KV1Element& addChild(KeyType key_, V value_ = {}) requires RequiresKeyConversion {
		if constexpr(UsesSTRREF) {
			KV1Element<KeyType, ValueType> elem{this->string_manager};
			this->children.push_back(elem);
		} else {
			KV1Element<KeyType, ValueType> elem{};
			this->children.push_back(elem);
		}
		KV1Element& elem = this->children.back();
		elem.setKey(key_);
		elem.setValue(value_);
		return elem;
	}

	/// Add a new child element with the given key, with a value and conditional
	template<KV1AllowedKeyTypes K, KV1ValueType V>
	KV1Element& addChild(K key_, V value_, const std::string& conditional_) requires RequiresKeyConversion {
		if constexpr(UsesSTRREF) {
			KV1Element<KeyType, ValueType> elem{this->string_manager};
			this->children.push_back(elem);
		} else {
			KV1Element<KeyType, ValueType> elem{};
			this->children.push_back(elem);
		}
		KV1Element& elem = this->children.back();
		elem.setKey(key_);
		elem.setValue(value_);
		elem.setConditional(conditional_);
		return elem;
	}

	/// Get the number of child elements
	[[nodiscard]] uint64_t getChildCount() const {
		return this->children.size();
	}

	/// Get the number of child elements with the given key
	template<KV1AllowedKeyTypes K>
	[[nodiscard]] uint64_t getChildCount(K childKey) const requires RequiresKeyComparison {
		uint64_t count = 0;
		for (const auto& element : this->children) {
			if (CompareKeys(element.key, childKey)) {
				++count;
			}
		}
		return count;
	}

	/// Get the child elements of the element
	[[nodiscard]] const std::vector<KV1Element>& getChildren() const {
		return this->children;
	}

	/// Get the child elements of the element
	[[nodiscard]] std::vector<KV1Element>& getChildren() {
		return this->children;
	}

	using iterator = std::vector<KV1Element>::iterator;

	[[nodiscard]] constexpr iterator begin() {
		return this->children.begin();
	}

	[[nodiscard]] constexpr iterator end() {
		return this->children.end();
	}

	using const_iterator = std::vector<KV1Element>::const_iterator;

	[[nodiscard]] constexpr const_iterator begin() const {
		return this->children.begin();
	}

	[[nodiscard]] constexpr const_iterator end() const {
		return this->children.end();
	}

	[[nodiscard]] constexpr const_iterator cbegin() const {
		return this->children.cbegin();
	}

	[[nodiscard]] constexpr const_iterator cend() const {
		return this->children.cend();
	}

	/// Get the child element of the element at the given index
	[[nodiscard]] const KV1Element& operator[](unsigned int n) const {
		return this->children.at(n);
	}

	/// Get the child element of the element at the given index
	[[nodiscard]] KV1Element& operator[](unsigned int n) {
		return this->children.at(n);
	}

	/// Get the first child element of the element with the given key
	template<KV1AllowedKeyTypes K>
	[[nodiscard]] const KV1Element& operator[](K childKey) const requires RequiresKeyComparison {
		return this->operator()(childKey);
	}

	/// Get the first child element of the element with the given key, or create a new element if it doesn't exist
	template<KV1AllowedKeyTypes K>
	[[nodiscard]] KV1Element& operator[](K childKey) requires RequiresKeyComparison {
		return this->operator()(childKey);
	}

	/// Get the first child element of the element with the given key
	template<KV1AllowedKeyTypes K>
	[[nodiscard]] const KV1Element& operator()(K childKey) const requires RequiresKeyComparison {
		for (const auto& element : this->children) {
			if (CompareKeys(element.key, childKey)) {
				return element;
			}
		}
		return getInvalid();
	}

	/// Get the first child element of the element with the given key, or create a new element if it doesn't exist
	template<KV1AllowedKeyTypes K>
	[[nodiscard]] KV1Element& operator()(K childKey) requires RequiresKeyComparison && RequiresKeyConversion {
		for (auto& element : this->children) {
			if (CompareKeys(element.key, childKey)) {
				return element;
			}
		}
		return this->template addChild<K, ValueType>(childKey);
	}

	/// Get the nth child element of the element with the given key
	template<KV1AllowedKeyTypes K>
	[[nodiscard]] const KV1Element& operator()(K childKey, unsigned int n) const requires RequiresKeyComparison && RequiresKeyConversion {
		unsigned int count = 0;
		for (const auto& element : this->children) {
			if (CompareKeys(element.key, childKey)) {
				if (count == n) {
					return element;
				}
				if (++count > n) {
					break;
				}
			}
		}
		return getInvalid();
	}

	/// Get the nth child element of the element with the given key, or create a new element if it doesn't exist
	template<KV1AllowedKeyTypes K>
	[[nodiscard]] KV1Element& operator()(K childKey, unsigned int n) requires RequiresKeyComparison && RequiresKeyConversion {
		unsigned int count = 0;
		for (auto& element: this->children) {
			if (CompareKeys(element.key, childKey)) {
				if (count == n) {
					return element;
				}
				if (++count > n) {
					break;
				}
			}
		}
		return this->template addChild<K, ValueType>(childKey);
	}

	/// Remove a child element from the element.
	void removeChild(unsigned int n) {
		if (this->children.size() > n) {
			this->children.erase(this->children.begin() + n);
		}
	}

	/// Remove a child element from the element with the given key. -1 means all children with the given key
	template<KV1AllowedKeyTypes K = KeyType>
	void removeChild(K childKey, int n = -1) requires RequiresKeyComparison {
		if (n == 0) { return; }
		unsigned int count = 0;

		std::erase_if(this->children,
			[&childKey, &count, n](const KV1Element& element) mutable noexcept -> bool {
				if (count > n && n != -1) { return false; }
				bool result = CompareKeys(element.key, childKey);
				if (result) { ++count; }
				return result;
			});
	}

	/// Check if the given element is invalid
	[[nodiscard]] bool isInvalid() const {
		return this == &getInvalid();
	}

	static const KV1Element& getInvalid() {
		static KV1Element element;
		return element;
	}

	//[[nodiscard]] explicit operator bool() const {
	//	return !this->isInvalid();
	//}

protected:
	KV1Element() = default;
	explicit KV1Element(std::shared_ptr<stringpp::string_reference_manager> mgr)
	requires std::same_as<KeyType, strref> || std::same_as<ValueType, strref>
	: string_manager(mgr) {
		// Make sure we set the manager
		if constexpr (std::same_as<KeyType, strref>) {
			this->key = mgr->request("");
		}
		if constexpr (std::same_as<ValueType, strref>) {
			this->value = mgr->request("");
		}
	}

public:
	/// Transfer between template specializations
	/// If you're transfering from a type that doesn't use strrefs, but this type uses strrefs, you can:
	/// 1. Pass an already made manager
	/// 2. Not pass it, and a new one will be created (you can always retrieve it with a method).
	template<KV1AllowedKeyTypes K, KV1UnderlayingValueType V>
	KV1Element(const KV1Element<K, V>& other, std::shared_ptr<stringpp::string_reference_manager> mgr = {}) {
		if constexpr (UsesSTRREF) {
			if constexpr (std::same_as<K, strref> || std::same_as<V, strref>) {
				this->string_manager = other.getSTRREFManager();
			} else {
				if (mgr != nullptr) {
					// Adopt
					this->string_manager = mgr;
				} else {
					// Create a new one
					this->string_manager = std::make_unique<stringpp::string_reference_manager>(true);
				}
			}

			this->key = this->string_manager->request(other.getKey());
			this->value = this->string_manager->request(other.getValue());

		} else {
			this->key = other.getKey();
			this->value = other.getValue();
		}

		for (auto c : other) {
			this->children.emplace_back(c, mgr);
		}
	}
protected:

	static void read(BufferStreamReadOnly& stream, std::vector<KV1Element>& elements, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences) requires
			(!(std::same_as<KeyType, strref> || std::same_as<ValueType, strref>)) { // Cannot use this when we are using a strref to hold any data
		using namespace sourcepp;
		while (true) {
			// Check if the block is over
			parser::text::eatWhitespaceAndSingleLineComments(stream);
			if (stream.peek<char>() == '}') {
				stream.skip();
				break;
			}
			// Read key
			{
				elements.push_back(KV1Element{});
				elements.back().key = parser::text::readString(stream, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read value
			if (stream.peek<char>() != '{') {
				elements.back().value = parser::text::readString(stream, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read conditional
			if (stream.peek<char>() == '[') {
				elements.back().conditional = parser::text::readString(stream, "[", "]", escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read block
			if (stream.peek<char>() == '{') {
				stream.skip();
				parser::text::eatWhitespaceAndSingleLineComments(stream);
				if (stream.peek<char>() != '}') {
					KV1Element::read(stream, elements.back().children, escapeSequences);
				} else {
					stream.skip();
				}
			}
		}
	}

	static void read(std::shared_ptr<stringpp::string_reference_manager> mgr, BufferStreamReadOnly& stream, std::vector<KV1Element>& elements, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences) requires
		(std::same_as<KeyType, strref> || std::same_as<ValueType, strref>) { // Used when we use strref
		using namespace sourcepp;
		while (true) {
			// Check if the block is over
			parser::text::eatWhitespaceAndSingleLineComments(stream);
			if (stream.peek<char>() == '}') {
				stream.skip();
				break;
			}
			// Read key
			{
				elements.push_back(KV1Element{mgr});
				elements.back().key = parser::text::readString(stream, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read value
			if (stream.peek<char>() != '{') {
				elements.back().value = parser::text::readString(stream, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read conditional
			if (stream.peek<char>() == '[') {
				elements.back().conditional = parser::text::readString(stream, "[", "]", escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read block
			if (stream.peek<char>() == '{') {
				stream.skip();
				parser::text::eatWhitespaceAndSingleLineComments(stream);
				if (stream.peek<char>() != '}') {
					KV1Element::read(mgr, stream, elements.back().children, escapeSequences);
				} else {
					stream.skip();
				}
			}
		}
		}

	static void write(BufferStream& stream, const std::vector<KV1Element>& elements, unsigned short indentLevel, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences) {
		using namespace sourcepp;
		constexpr auto writeIndentation = [](BufferStream& stream_, unsigned short indentLevel_) {
			for (unsigned short i = 0; i < indentLevel_; i++) {
				stream_.write('\t');
			}
		};
		constexpr auto writeQuotedString = [](BufferStream& stream_, std::string_view str, const parser::text::EscapeSequenceMap& escapeSequences_, char quoteStart = '\"', char quoteEnd = '\"') {
			stream_.write(quoteStart);
			if (!str.empty()) {
				stream_.write(parser::text::convertSpecialCharsToEscapes(str, escapeSequences_), false);
			}
			stream_.write(quoteEnd);
		};

		for (const auto& e : elements) {
			writeIndentation(stream, indentLevel);
			writeQuotedString(stream, e.key, escapeSequences);
			if (!e.value.empty() || e.children.empty()) {
				stream.write(' ');
				writeQuotedString(stream, e.value, escapeSequences);
			}
			if (!e.conditional.empty()) {
				stream.write(' ');
				writeQuotedString(stream, e.conditional, escapeSequences, '[', ']');
			}
			stream.write('\n');
			if (!e.children.empty()) {
				writeIndentation(stream, indentLevel);
				stream << '{' << '\n';
				write(stream, e.children, indentLevel + 1, escapeSequences);
				writeIndentation(stream, indentLevel);
				stream << '}' << '\n';
			}
		}
	}

	KeyType key; // NOLINT(*-redundant-string-init)
	ValueType value; // NOLINT(*-redundant-string-init)
	std::string conditional; // NOLINT(*-redundant-string-init)
	std::vector<KV1Element<KeyType, ValueType>> children;


	struct empty {};
	using needs_strref_mgr = std::conditional_t<std::is_same_v<KeyType, strref> || std::is_same_v<ValueType, strref>,
	std::shared_ptr<stringpp::string_reference_manager>, empty>;
#if defined(_MSC_VER) // Seriously????
	[[msvc::no_unique_address]] needs_strref_mgr string_manager;
#else
	[[no_unique_address]] needs_strref_mgr string_manager;
#endif
};

/// Class representing a KeyValue 1 tree object.
/// You can use different (string-based) types for Keys and Values
template<KV1AllowedKeyTypes KeyType = stringpp::string_reference, KV1UnderlayingValueType ValueType = stringpp::string_reference>
requires (!((std::same_as<KeyType, std::string_view> && std::same_as<ValueType, stringpp::string_reference>) ||
	(std::same_as<KeyType, stringpp::string_reference> && std::same_as<ValueType, std::string_view>)))
class KV1 : public KV1Element<KeyType, ValueType> {
	using strref = stringpp::string_reference;
public:
	/// Constructor used for text based keyvalues
	/// This should be used for KV1s that use deepcopy-based strings, such as std::string
	explicit KV1(std::string_view kv1Data = "", const bool useEscapeSequences = false)
	requires (std::is_convertible_v<KeyType, std::string> && std::is_convertible_v<ValueType, std::string> // Requires the values to be able to be represented as strings
			&& !UsesSTRREF  // But not any string_reference
			):
			KV1Element<KeyType, ValueType>() {
		// Skip the UTF-8 BOM
		if (kv1Data.starts_with("\xEF\xBB\xBF")) {
			kv1Data.remove_prefix(3);
		}
		if (kv1Data.empty()) {
			return;
		}
		BufferStreamReadOnly stream{kv1Data};
		try {
			KV1Element<KeyType, ValueType>::read(stream, this->children, sourcepp::parser::text::getDefaultEscapeSequencesOrNone(useEscapeSequences));
		} catch (const std::overflow_error&) {}
	}

	/// Used to construct KV1 from data, if we specified that we're gonna use String Reference.
	/// You will need to create the manager yourself!
	explicit KV1(std::shared_ptr<stringpp::string_reference_manager> mgr, std::string_view kv1Data = "", const bool useEscapeSequences = false)
	requires UsesSTRREF:
			KV1Element<KeyType, ValueType>(mgr) {
		this->string_manager = mgr;
		// Skip the UTF-8 BOM
		if (kv1Data.starts_with("\xEF\xBB\xBF")) {
			kv1Data.remove_prefix(3);
		}
		if (kv1Data.empty()) {
			return;
		}
		BufferStreamReadOnly stream{kv1Data};
		try {
			KV1Element<KeyType, ValueType>::read(mgr, stream, this->children, sourcepp::parser::text::getDefaultEscapeSequencesOrNone(useEscapeSequences));
		} catch (const std::overflow_error&) {}
	}

	/// Create by new ownership
	explicit KV1(const KV1Element<KeyType, ValueType>& child) {
		this->key = child.getKey();
		this->children = child.getChildren();
		this->value = child; // uses operator overloads
		if constexpr (UsesSTRREF) {
			this->string_manager = child.getSTRREFManager();
		}
	}

	/// Transfer between template specializations
	/// If you're transfering from a type that doesn't use strrefs, but this type uses strrefs, you can:
	/// 1. Pass an already made manager
	/// 2. Not pass it, and a new one will be created (you can always retrieve it with a method).
	template<KV1AllowedKeyTypes K, KV1UnderlayingValueType V>
	KV1(const KV1<K, V>& other, std::shared_ptr<stringpp::string_reference_manager> mgr = {}) {
		if constexpr (UsesSTRREF) {
			if constexpr (std::same_as<K, strref> || std::same_as<V, strref>) {
				this->string_manager = other.getSTRREFManager();
			} else {
				if (mgr != nullptr) {
					// Adopt
					this->string_manager = mgr;
				} else {
					// Create a new one
					this->string_manager = std::make_unique<stringpp::string_reference_manager>(true);
				}
			}

			this->key = this->string_manager->request(other.getKey());
			this->value = this->string_manager->request(other.getValue());

		} else {
			this->key = other.key;
			this->value = other.value;
		}

		// Transfer children
		for (auto c : other.children) {
			this->children.emplace_back(c, this->string_manager);
		}
	}

	[[nodiscard]] std::string bake(const bool useEscapeSequences = false) const {
		std::string buffer;
		BufferStream stream{buffer};
		KV1Element<KeyType, ValueType>::write(stream, this->children, 0, sourcepp::parser::text::getDefaultEscapeSequencesOrNone(useEscapeSequences));
		buffer.resize(stream.size());
		return buffer;
	}

	void bake(const std::filesystem::path& kv1Path) const {
		sourcepp::fs::writeFileText(kv1Path, this->bake());
	}
};
} // namespace kvpp

#undef RequiresKeyComparison
#undef RequiresKeyConversion
#undef UsesSTRREF
