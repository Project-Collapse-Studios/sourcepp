#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <vector>

#include <BufferStream.h>
#include <sourcepp/parser/Text.h>
#include <sourcepp/String.h>

#include <sourcepp/Math.h>

#include <format>
#include <sourcepp/FS.h>

namespace kvpp {

// Definitions for KV1Writer
template<typename S = std::string>
requires std::convertible_to<S, std::string_view>
class KV1Writer;

template<typename S = std::string>
requires std::convertible_to<S, std::string_view>
class KV1ElementWritable;
// END

template <typename T>
struct is_math_vec : std::false_type {};

template<uint8_t S, mathpp::Arithmetic P>
struct is_math_vec<mathpp::Vec<S, P>> : std::true_type {};

template<typename V>
concept KV1ValueType = std::convertible_to<V, std::string_view>
                    || std::same_as<V, bool>
                    || std::same_as<V, int32_t>
                    || std::same_as<V, int64_t>
                    || std::same_as<V, float>
					|| is_math_vec<V>::value;

template<typename S = std::string_view>
requires std::convertible_to<S, std::string_view>
class KV1ElementReadable {
public:

	/// Convert from KV1ElementWritable
	explicit KV1ElementReadable(const KV1ElementWritable<S>& other) {
		if (other.getChildCount()) {
			for (auto element : other.getChildren()) {
				this->children.push_back(KV1ElementReadable<S>(element));
			}
		} else {
			this->value = other.getValue();
			this->key = other.getKey();
			this->conditional = other.getConditional();
		}
	}


	/// Get the key associated with the element
	[[nodiscard]] std::string_view getKey() const {
		return this->key;
	}

	/// Get the value associated with the element
	[[nodiscard]] std::string_view getValue() const {
		return this->value;
	}

	/// Get the value associated with the element as the given type
	template<KV1ValueType V>
	[[nodiscard]] V getValue() const {
		if constexpr (std::convertible_to<V, std::string_view>) {
			return this->value;
		} else if constexpr (std::same_as<V, bool>) {
			return static_cast<bool>(this->getValue<int32_t>());
		} else if constexpr (std::same_as<V, int32_t> || std::same_as<V, int64_t>) {
			V out = 0;
			if (this->value.length() == 10 && this->value.starts_with("0x") && sourcepp::parser::text::isNumber(this->value.substr(2))) {
				sourcepp::string::toInt(this->value.substr(2), out, 16);
			} else {
				sourcepp::string::toInt(this->value, out);
			}
			return out;
		} else if constexpr (std::same_as<V, float>) {
			float out = 0.f;
			sourcepp::string::toFloat(this->value, out);
			return out;
		} else if constexpr (is_math_vec<V>::value) {
			auto splits = sourcepp::string::split(this->value, ' ');
			auto return_val = V{};
			for (uint8_t i = 0; i < return_val.S; ++i) {
				if (i >= splits.size()) {
					return_val.values[i] = 0;
				}
				sourcepp::string::toArithmetic<typename V::value_type>(this->value, return_val.values[i]);
			}
			return return_val;
		}
		return V{};
	}

	/// Get the conditional associated with the element
	[[nodiscard]] std::string_view getConditional() const {
		return this->conditional;
	}

	/// Check if the element has one or more children with the given key
	[[nodiscard]] bool hasChild(const std::string_view childKey) const {
		return !this->operator[](childKey).isInvalid();
	}

	/// Get the number of child elements
	[[nodiscard]] uint64_t getChildCount() const {
		return this->children.size();
	}

	/// Get the number of child elements with the given key
	[[nodiscard]] uint64_t getChildCount(const std::string_view childKey) const {
		uint64_t count = 0;
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.key, childKey)) {
				++count;
			}
		}
		return count;
	}

	/// Get the child elements of the element
	[[nodiscard]] const std::vector<KV1ElementReadable>& getChildren() const {
		return this->children;
	}

	using iterator = std::vector<KV1ElementReadable>::iterator;

	[[nodiscard]] constexpr iterator begin() {
		return this->children.begin();
	}

	[[nodiscard]] constexpr iterator end() {
		return this->children.end();
	}

	using const_iterator = std::vector<KV1ElementReadable>::const_iterator;

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
	[[nodiscard]] const KV1ElementReadable& operator[](unsigned int n) const {
		return this->children.at(n);
	}

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1ElementReadable& operator[](const std::string_view childKey) const {
		return this->operator()(childKey);
	}

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1ElementReadable& operator()(const std::string_view childKey) const {
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
				return element;
			}
		}
		return getInvalid();
	}

	/// Get the nth child element of the element with the given key
	[[nodiscard]] const KV1ElementReadable& operator()(const std::string_view childKey, const unsigned int n) const {
		unsigned int count = 0;
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
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

	/// Check if the given element is invalid
	[[nodiscard]] bool isInvalid() const {
		return this == &getInvalid();
	}

	static const KV1ElementReadable& getInvalid() {
		static KV1ElementReadable element;
		return element;
	}

	[[nodiscard]] explicit operator bool() const {
		return !this->isInvalid();
	}

protected:
	KV1ElementReadable() = default;

	static void read(BufferStreamReadOnly& stream, BufferStream& backing, std::vector<KV1ElementReadable>& elements, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences) {
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
				elements.push_back(KV1ElementReadable{});
				elements.back().key = parser::text::readStringToBuffer(stream, backing, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read value
			if (stream.peek<char>() != '{') {
				elements.back().value = parser::text::readStringToBuffer(stream, backing, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read conditional
			if (stream.peek<char>() == '[') {
				elements.back().conditional = parser::text::readStringToBuffer(stream, backing, "[", "]", escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read block
			if (stream.peek<char>() == '{') {
				stream.skip();
				parser::text::eatWhitespaceAndSingleLineComments(stream);
				if (stream.peek<char>() != '}') {
					KV1ElementReadable::read(stream, backing, elements.back().children, escapeSequences);
				} else {
					stream.skip();
				}
			}
		}
	}

	S key = ""; // NOLINT(*-redundant-string-init)
	S value = ""; // NOLINT(*-redundant-string-init)
	S conditional = ""; // NOLINT(*-redundant-string-init)
	std::vector<KV1ElementReadable> children;
};

template<typename S = std::string_view>
requires std::convertible_to<S, std::string_view>
class KV1 : public KV1ElementReadable<S> {
public:
	explicit KV1(std::string_view kv1Data, const bool useEscapeSequences_ = false)
			: KV1ElementReadable<S>()
			, useEscapeSequences(useEscapeSequences_) {
		// Skip the UTF-8 BOM
		if (kv1Data.starts_with("\xEF\xBB\xBF")) {
			kv1Data.remove_prefix(3);
		}
		if (kv1Data.empty()) {
			return;
		}
		BufferStreamReadOnly stream{kv1Data};

		// Multiply by 2 to ensure buffer will have enough space (very generous)
		this->backingData.resize(kv1Data.size() * 2);
		BufferStream backing{this->backingData, false};
		try {
			KV1ElementReadable<S>::read(stream, backing, this->children, sourcepp::parser::text::getDefaultEscapeSequencesOrNone(this->useEscapeSequences));
		} catch (const std::overflow_error&) {}
	}

	explicit KV1(const KV1ElementReadable<S>& kv1Data, const bool useEscapeSequences_ = false) : useEscapeSequences(useEscapeSequences_) {
		this->children = kv1Data.getChildren();
		this->key = kv1Data.getKey();
	}


	friend class KV1Writer<S>; // Befriend the class, but only of the same type

	KV1(const KV1Writer<S>& other) {
		this->useEscapeSequences = other.useEscapeSequences;
		for (auto element : other.getChildren()) {
			this->children.push_back(KV1ElementReadable<S>(element));
		}
	}


protected:
	using KV1ElementReadable<S>::getKey;
	using KV1ElementReadable<S>::getValue;
	using KV1ElementReadable<S>::getConditional;

	std::string backingData;
	bool useEscapeSequences;
};

namespace literals {

inline KV1<> operator""_kv1(const char* str, const std::size_t len) {
	return KV1{std::string_view{str, len}};
}

} // namespace literals


// KV1Writer
template<typename S> // Default argument defined in KV1.h
requires std::convertible_to<S, std::string_view>
class KV1ElementWritable {
public:
	/// Allow to convert KV1ElementReadable onto us.
	KV1ElementWritable(const KV1ElementReadable<S>& other) {
		if (other.getChildCount()) {
			for (auto element : other.getChildren()) {
				this->children.push_back(KV1ElementWritable<S>(element));
			}
		} else {
			this->value = other.getValue();
			this->key = other.getKey();
			this->conditional = other.getConditional();
		}
	}

	/// Get the key associated with the element
	[[nodiscard]] std::string_view getKey() const {
		return this->key;
	}

	/// Set the key associated with the element
	void setKey(std::string_view key_) {
		this->key = key_;
	}

	/// Get the value associated with the element
	[[nodiscard]] std::string_view getValue() const {
		return this->value;
	}

	/// Get the value associated with the element as the given type
	template<KV1ValueType V>
	[[nodiscard]] V getValue() const {
		if constexpr (std::convertible_to<V, std::string_view>) {
			return this->value;
		} else if constexpr (std::same_as<V, bool>) {
			return static_cast<bool>(this->getValue<int32_t>());
		} else if constexpr (std::same_as<V, int32_t> || std::same_as<V, int64_t>) {
			V out = 0;
			if (this->value.length() == 10 && this->value.starts_with("0x") && sourcepp::parser::text::isNumber(this->value.substr(2))) {
				sourcepp::string::toInt(this->value.substr(2), out, 16);
			} else {
				sourcepp::string::toInt(this->value, out);
			}
			return out;
		} else if constexpr (std::same_as<V, float>) {
			float out = 0.f;
			sourcepp::string::toFloat(this->value, out);
			return out;
		}  else if constexpr (is_math_vec<V>::value) {
			auto splits = sourcepp::string::split(this->value, ' ');
			auto return_val = V{};
			for (uint8_t i = 0; i < return_val.S; ++i) {
				if (i >= splits.size()) {
					return_val.values[i] = 0;
				}
				sourcepp::string::toArithmetic<typename V::value_type>(splits[i], return_val.values[i]);
			}
			return return_val;
		}
		return V{};
	}

	/// Set the value associated with the element
	template<KV1ValueType V>
	void setValue(V value_) {
		if constexpr (std::convertible_to<V, std::string_view>) {
			this->value = std::string_view{value_};
		} else if constexpr (std::same_as<V, bool>) {
			this->setValue(std::format("{:b}", value_));
		} else {
			this->setValue(std::format("{}", value_));
		}
	}

	/// Set the value associated with the element
	template<KV1ValueType V>
	KV1ElementWritable& operator=(V value_) {
		this->setValue(value_);
		return *this;
	}

	/// Get the conditional associated with the element
	[[nodiscard]] std::string_view getConditional() const {
		return this->conditional;
	}

	/// Set the conditional associated with the element
	void setConditional(std::string_view conditional_) {
		this->conditional = conditional_;
	}

	/// Check if the element has one or more children with the given key
	[[nodiscard]] bool hasChild(std::string_view childKey) const {
		return !this->operator[](childKey).isInvalid();
	}

	// Add a new child element with the given key, with an optional value and/or conditional
	template<KV1ValueType V = std::string_view>
	KV1ElementWritable& addChild(std::string_view key_, V value_ = {}, std::string_view conditional_ = "") {
		KV1ElementWritable elem;
		elem.setKey(key_);
		elem.setValue(value_);
		elem.setConditional(conditional_);
		this->children.push_back(elem);
		return this->children.back();
	}

	/// Get the number of child elements
	[[nodiscard]] uint64_t getChildCount() const {
		return this->children.size();
	}

	/// Get the number of child elements with the given key
	[[nodiscard]] uint64_t getChildCount(std::string_view childKey) const {
		uint64_t count = 0;
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.key, childKey)) {
				++count;
			}
		}
		return count;
	}

	/// Get the child elements of the element
	[[nodiscard]] const std::vector<KV1ElementWritable>& getChildren() const {
		return this->children;
	}

	/// Get the child elements of the element
	[[nodiscard]] std::vector<KV1ElementWritable>& getChildren() {
		return this->children;
	}

	using iterator = std::vector<KV1ElementWritable>::iterator;

	[[nodiscard]] constexpr iterator begin() {
		return this->children.begin();
	}

	[[nodiscard]] constexpr iterator end() {
		return this->children.end();
	}

	using const_iterator = std::vector<KV1ElementWritable>::const_iterator;

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
	[[nodiscard]] const KV1ElementWritable& operator[](unsigned int n) const {
		return this->children.at(n);
	}

	/// Get the child element of the element at the given index
	[[nodiscard]] KV1ElementWritable& operator[](unsigned int n) {
		return this->children.at(n);
	}

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1ElementWritable& operator[](std::string_view childKey) const {
		return this->operator()(childKey);
	}

	/// Get the first child element of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV1ElementWritable& operator[](std::string_view childKey) {
		return this->operator()(childKey);
	}

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1ElementWritable& operator()(std::string_view childKey) const {
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
				return element;
			}
		}
		return getInvalid();
	}

	/// Get the first child element of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV1ElementWritable& operator()(std::string_view childKey) {
		for (auto& element : this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
				return element;
			}
		}
		return this->addChild(childKey);
	}

	/// Get the nth child element of the element with the given key
	[[nodiscard]] const KV1ElementWritable& operator()(std::string_view childKey, unsigned int n) const {
		unsigned int count = 0;
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
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
	[[nodiscard]] KV1ElementWritable& operator()(std::string_view childKey, unsigned int n) {
		unsigned int count = 0;
		for (auto& element: this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
				if (count == n) {
					return element;
				}
				if (++count > n) {
					break;
				}
			}
		}
		return this->addChild(childKey);
	}

	/// Remove a child element from the element.
	void removeChild(unsigned int n) {
		if (this->children.size() > n) {
			this->children.erase(this->children.begin() + n);
		}
	}

	/// Remove a child element from the element with the given key. -1 means all children with the given key
	void removeChild(std::string_view childKey, int n = -1) {
		if (n == 0) { return; }
		unsigned int count = 0;

		std::erase_if(this->children,
			[&childKey, &count, n](const KV1ElementWritable& element) mutable noexcept -> bool {
				if (count > n && n != -1) { return false; }
				bool result = sourcepp::string::iequals(element.getKey(), childKey);
				if (result) { ++count; }
				return result;
			});
	}

	/// Check if the given element is invalid
	[[nodiscard]] bool isInvalid() const {
		return this == &getInvalid();
	}

	static const KV1ElementWritable& getInvalid() {
		static KV1ElementWritable element;
		return element;
	}

	[[nodiscard]] explicit operator bool() const {
		return !this->isInvalid();
	}

protected:
	KV1ElementWritable() = default;

	static void read(BufferStreamReadOnly& stream, std::vector<KV1ElementWritable>& elements, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences) {
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
				elements.push_back(KV1ElementWritable{});
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
					KV1ElementWritable::read(stream, elements.back().children, escapeSequences);
				} else {
					stream.skip();
				}
			}
		}
	}

	static void write(BufferStream& stream, const std::vector<KV1ElementWritable>& elements, unsigned short indentLevel, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences) {
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

	S key = ""; // NOLINT(*-redundant-string-init)
	S value = ""; // NOLINT(*-redundant-string-init)
	S conditional = ""; // NOLINT(*-redundant-string-init)
	std::vector<KV1ElementWritable> children;
};

template<typename S> // Default defined in KV1.h
requires std::convertible_to<S, std::string_view>
class KV1Writer : public KV1ElementWritable<S> {
public:
	explicit KV1Writer(std::string_view kv1Data = "", bool useEscapeSequences_ = false)
			: KV1ElementWritable<S>()
			, useEscapeSequences(useEscapeSequences_) {
		// Skip the UTF-8 BOM
		if (kv1Data.starts_with("\xEF\xBB\xBF")) {
			kv1Data.remove_prefix(3);
		}
		if (kv1Data.empty()) {
			return;
		}
		BufferStreamReadOnly stream{kv1Data};
		try {
			KV1ElementWritable<S>::read(stream, this->children, sourcepp::parser::text::getDefaultEscapeSequencesOrNone(this->useEscapeSequences));
		} catch (const std::overflow_error&) {}
	}

	/// Constructor used to cast KV1ElementWritable into a KV1Writer
	KV1Writer(const KV1ElementWritable<S>& kv1Data, bool useEscapeSequences_ = false) : useEscapeSequences(useEscapeSequences_) {
		this->children = kv1Data.getChildren();
		this->key = kv1Data.getKey();
	}

	friend class KV1<S>; // Befriend the class, but only of the same type

	/// Construct from a read-only KV1<S>
	KV1Writer(const KV1<S>& other) {
		this->useEscapeSequences = other.useEscapeSequences;
		for (auto element : other.getChildren()) {
			this->children.push_back(KV1ElementWritable<S>(element));
		}
	}

	[[nodiscard]] std::string bake() const {
		std::string buffer;
		BufferStream stream{buffer};
		KV1ElementWritable<S>::write(stream, this->children, 0, sourcepp::parser::text::getDefaultEscapeSequencesOrNone(this->useEscapeSequences));
		buffer.resize(stream.size());
		return buffer;
	}

	void bake(const std::filesystem::path& kv1Path) const {
		sourcepp::fs::writeFileText(kv1Path, this->bake());
	}

protected:
	using KV1ElementWritable<S>::getKey;
	using KV1ElementWritable<S>::setKey;
	using KV1ElementWritable<S>::getValue;
	using KV1ElementWritable<S>::setValue;
	using KV1ElementWritable<S>::operator=;
	using KV1ElementWritable<S>::getConditional;
	using KV1ElementWritable<S>::setConditional;

	bool useEscapeSequences;
};

namespace literals {

inline KV1Writer<> operator""_kv1w(const char* str, const std::size_t len) {
	return KV1Writer{std::string_view{str, len}};
}

} // namespace literals

template<typename V>
concept AnyKV1Element = std::same_as<V, KV1ElementReadable<>> ||
						std::same_as<V, KV1ElementWritable<>>;

} // namespace kvpp
