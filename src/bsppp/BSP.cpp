// ReSharper disable CppRedundantParentheses

#include <bsppp/BSP.h>


#include <filesystem>

using namespace bsppp;
using namespace sourcepp;

namespace bsppp {

BSP::BSP(const std::filesystem::path& path, bool loadPatchFiles): input_stream{path} {

	if (!this->input_stream.value()) {
		throw std::runtime_error("Cannot open specified file!");
	}


	// Parse the header

	uint32_t magic_keyword =  this->input_stream->read<uint32_t>();
	if (magic_keyword == BSP_SIGNATURE) {
		this->endianess = std::endian::little;
	} else if (magic_keyword == BSP_CONSOLE_SIGNATURE) {
		this->endianess = std::endian::big;
		this->input_stream->set_big_endian(true);
	} else {
		throw std::runtime_error("Unknown header structure in BSP file!");
	}

	this->version = this->input_stream->read<uint32_t>();

	// Contagion funny
	if (this->version == 27) {
		this->input_stream->skip_in<uint32_t>();
	}


	for (auto& [offset, length, version_, uncompressedLength] : this->lump_locations) {
		this->input_stream->read(offset)
			//>> offset
			>> length
			>> version_
			>> uncompressedLength;
	}

	//this->stagedVersion = this->header.version;
	//this->stagedGameLumps = this->parseGameLumps(false);
	//this->stagedMapRevision = this->header.mapRevision;

	// If no offsets are larger than 1024 (less than the size of the BSP header, but greater
	// than any lump version), it's probably a L4D2 BSP and those are lump versions!
	if (this->version == 21) {
		int i;
		for (i = 0; i < BSP_LUMP_COUNT; i++) {
			if (this->lump_locations[i].offset > 1024) {
				break;
			}
		}
		this->l4d2 = i == BSP_LUMP_COUNT;
		if (this->l4d2) {
			// Swap fields around
			for (i = 0; i < BSP_LUMP_COUNT; i++) {
				std::swap(this->lump_locations[i].offset, this->lump_locations[i].version);
				std::swap(this->lump_locations[i].offset, this->lump_locations[i].length);
			}
		}
	}

	this->input_stream->read(this->mapRevision);

	// TODO: Reimplement
	/*
	if (loadPatchFiles) {
		const auto fsPath = std::filesystem::path{this->path};
		const auto fsStem = (fsPath.parent_path() / fsPath.stem()).string();

		for (int i = 0; ; i++) {
			auto patchFilePath = std::format("{}_l_{}.lmp", fsStem, i);
			if (!std::filesystem::exists(patchFilePath)) {
				break;
			}
			this->setLumpFromPatchFile(patchFilePath);
		}
	}
	*/
}


BSP::BSP(uint32_t version_, uint32_t mapRevision_, std::endian endianess_):
	endianess(endianess_),
	version(version_),
	lump_locations(),
	mapRevision(mapRevision_) {

	// These get set when we're writing the file.
	for (auto& [offset, length, lversion, clength] : this->lump_locations) {
		offset = 0;
		length = 0;
		lversion = 0;
		clength = 0;
	}
}

uint32_t BSP::getVersion() const {
	return this->version;
}

void BSP::setVersion(uint32_t version_) {
	this->version = version_;
}

uint32_t BSP::getMapRevision() const {
	return this->mapRevision;
}

void BSP::setMapRevision(uint32_t mapRevision_) {
	this->mapRevision = mapRevision_;
}

bool BSP::isL4D2() const {
	return this->l4d2;
}

void BSP::setL4D2(bool isL4D2) {
	this->l4d2 = isL4D2;
}

std::endian BSP::getEndianess() const {
	return this->endianess;
}

void BSP::setEndianess(std::endian endian_) {
	this->endianess = endian_;
}

bool BSP::hasLump(const BSPLump lumpIndex) const {

	switch (lumpIndex) {
		case BSPLump::UNKNOWN:
			return false;

		case BSPLump::ENTITIES:
			if (this->lump_entities.has_value()) {
				return true;
			}
			break;

		default:
			return false;
	}

	// If the lump doesn't exist in the parsed data, does it exist in the bsp file?
	if (!this->input_stream.has_value()) {
		return false; // We weren't created by an input file
	}

	// If the offset is other than 0, then the lump exists in the file.
	return this->lump_locations[static_cast<uint8_t>(lumpIndex)].offset > 0;
}

void BSP::parseLump(const BSPLump lumpIndex) {
	if (!hasLump(lumpIndex)) {
		throw lump_doesnt_exist{};
	}

	auto offset = this->lump_locations[static_cast<uint8_t>(lumpIndex)].offset;
	auto length = this->lump_locations[static_cast<uint8_t>(lumpIndex)].length;
	auto version = this->lump_locations[static_cast<uint8_t>(lumpIndex)].version;

	this->input_stream->seek_in(offset); // Move the cursor to prepare for reading.

	switch (lumpIndex) {
		case BSPLump::UNKNOWN:
			return;

		case BSPLump::ENTITIES:
			if (this->lump_entities.has_value()) {return;}
			this->lump_entities = EntityLump{this->input_stream->read_string(length, false), version};
			return;

		default:
			return;
	}
}

//bool BSP::isLumpCompressed(BSPLump lumpIndex) const {
//	if (this->hasLump(lumpIndex)) {
//		const auto lump = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
//		return (this->stagedLumps.contains(lump) && this->stagedLumps.at(lump).first.uncompressedLength > 0) || this->header.lumps[lump].uncompressedLength > 0;
//	}
//	return false;
//}
//
//uint32_t BSP::getLumpVersion(BSPLump lumpIndex) const {
//	if (this->path.empty()) {
//		return 0;
//	}
//	const auto lump = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
//	if (this->stagedLumps.contains(lump)) {
//		return this->stagedLumps.at(lump).first.version;
//	}
//	return this->header.lumps[lump].version;
//}












//void BSP::createLumpPatchFile(BSPLump lumpIndex) const {
//	if (this->path.empty()) {
//		return;
//	}
//
//	const auto lumpData = this->getLumpData(lumpIndex);
//	if (!lumpData) {
//		return;
//	}
//
//	const auto& [
//		lumpOffset,
//		lumpLength,
//		lumpVersion,
//		lumpUncompressedLength
//	] = this->header.lumps.at(static_cast<std::underlying_type_t<BSPLump>>(lumpIndex));
//
//	const auto fsPath = std::filesystem::path{this->path};
//	const auto fsStem = (fsPath.parent_path() / fsPath.stem()).string();
//	int nonexistentNumber = 0;
//	while (true) {
//		if (!std::filesystem::exists(std::format("{}_l_{}.lmp", fsStem, nonexistentNumber))) {
//			break;
//		}
//		nonexistentNumber++;
//	}
//
//	FileStream writer{std::format("{}_l_{}.lmp", fsStem, nonexistentNumber), FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
//	writer
//		.seek_out(0)
//		.write<int32_t>(sizeof(int32_t) * 5)
//		.write(lumpIndex)
//		.write(lumpVersion)
//		.write(lumpLength)
//		.write(this->header.mapRevision)
//		.write(*lumpData);
//}

//bool BSP::setLumpFromPatchFile(const std::string& lumpFilePath) {
//	if (this->path.empty()) {
//		return false;
//	}
//
//	FileStream reader{lumpFilePath};
//	if (!reader) {
//		return false;
//	}
//
//	const auto offset = reader.read<uint32_t>();
//	const auto index = reader.read<uint32_t>();
//	const auto version = reader.read<uint32_t>();
//	const auto length = reader.read<uint32_t>();
//	if (index > BSP_LUMP_COUNT || offset == 0 || length == 0) {
//		return false;
//	}
//
//	this->setLump(static_cast<BSPLump>(index), version, reader.seek_in(offset).read_bytes(length));
//	return true;
//}

/*
bool BSP::bake(std::string_view outputPath) {
	if (this->path.empty()) {
		return false;
	}

	std::vector<std::byte> out;
	BufferStream stream{out};
	stream.set_big_endian(this->console);

	stream << BSP_SIGNATURE << this->stagedVersion;
	if (this->stagedVersion == 27) {
		// Contagion funny
		stream.write<uint32_t>(0);
	}

	const auto lumpsHeaderOffset = stream.tell();
	for (int i = 0; i < sizeof(Header::lumps); i++) {
		stream.write<uint8_t>(0);
	}

	stream << this->stagedMapRevision;

	for (auto i : BSP_LUMP_ORDER) {
		if (!this->hasLump(static_cast<BSPLump>(i))) {
			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + i * sizeof(LumpLocation));

			const auto& lump = this->header.lumps[i];
			if (!this->l4d2) {
				stream
					.write<uint32_t>(0)
					.write<uint32_t>(0)
					.write<uint32_t>(lump.version);
			} else {
				stream
					.write<uint32_t>(lump.version)
					.write<uint32_t>(0)
					.write<uint32_t>(0);
			}
			stream
				.write<uint32_t>(0)
				.seek_u(curPos);
			continue;
		}

		// Lumps are 4 byte aligned
		if (const auto padding = math::paddingForAlignment(4, stream.tell()); padding > 0) {
			stream.pad(padding);
		}

		if (static_cast<BSPLump>(i) == BSPLump::GAME_LUMP && !this->stagedGameLumps.empty()) {
			const auto gameLumpOffset = stream.tell();

			bool oneOrMoreGameLumpCompressed = false;
			for (const auto& gameLump : this->stagedGameLumps) {
				if (gameLump.isCompressed) {
					oneOrMoreGameLumpCompressed = true;
					break;
				}
			}
			// NOLINTNEXTLINE(*-sizeof-container)
			auto gameLumpCurrentOffset = !this->console * stream.tell() + sizeof(int32_t) + ((sizeof(BSPGameLump) - sizeof(BSPGameLump::data)) * (this->stagedGameLumps.size() + oneOrMoreGameLumpCompressed));
			stream.write<uint32_t>(this->stagedGameLumps.size() + oneOrMoreGameLumpCompressed);

			for (const auto& gameLump : this->stagedGameLumps) {
				if (gameLump.signature == 0) {
					break;
				}
				stream
					.write<uint32_t>(gameLump.signature)
					.write<uint16_t>(gameLump.isCompressed)
					.write<uint16_t>(gameLump.version)
					.write<uint32_t>(gameLumpCurrentOffset)
					.write<uint32_t>(gameLump.uncompressedLength);
				gameLumpCurrentOffset += gameLump.data.size();
			}
			if (oneOrMoreGameLumpCompressed) {
				stream
					.write<uint32_t>(0)
					.write<uint16_t>(0)
					.write<uint16_t>(0)
					.write<uint32_t>(gameLumpCurrentOffset)
					.write<uint32_t>(0);
			}

			for (const auto& gameLump : this->stagedGameLumps) {
				if (gameLump.signature == 0) {
					break;
				}
				stream.write(gameLump.data);
			}

			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + i * sizeof(LumpLocation));
			if (!this->l4d2) {
				stream
					.write<uint32_t>(gameLumpOffset)
					.write<uint32_t>(curPos - gameLumpOffset)
					.write<uint32_t>(0);
			} else {
				stream
					.write<uint32_t>(0)
					.write<uint32_t>(gameLumpOffset)
					.write<uint32_t>(curPos - gameLumpOffset);
			}
			stream
				.write<uint32_t>(0)
				.seek_u(curPos);
			continue;
		}

		if (this->stagedLumps.contains(i)) {
			const auto&[lump, lumpData] = this->stagedLumps.at(i);
			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + i * sizeof(LumpLocation));
			if (!this->l4d2) {
				stream
					.write<uint32_t>(curPos)
					.write<uint32_t>(lumpData.size())
					.write<uint32_t>(lump.version);
			} else {
				stream
					.write<uint32_t>(lump.version)
					.write<uint32_t>(curPos)
					.write<uint32_t>(lumpData.size());
			}
			stream
				.write<uint32_t>(lump.uncompressedLength)
				.seek_u(curPos)
				.write(lumpData);
			continue;
		}

		if (const auto data = this->getLumpData(static_cast<BSPLump>(i), true)) {
			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + i * sizeof(LumpLocation));

			const auto& lump = this->header.lumps[i];
			if (!this->l4d2) {
				stream
					.write<uint32_t>(curPos)
					.write<uint32_t>(lump.length)
					.write<uint32_t>(lump.version);
			} else {
				stream
					.write<uint32_t>(lump.version)
					.write<uint32_t>(curPos)
					.write<uint32_t>(lump.length);
			}
			stream
				.write<uint32_t>(lump.uncompressedLength)
				.seek_u(curPos)
				.write(*data);
		} else {
			// We should never be here!
			SOURCEPP_DEBUG_BREAK;
		}
	}

	// Lumps are 4 byte aligned
	if (const auto padding = math::paddingForAlignment(4, stream.tell()); padding > 0) {
		stream.pad(padding);
	}

	out.resize(stream.size());
	if (!fs::writeFileBuffer(outputPath.empty() ? this->path : std::string{outputPath}, out)) {
		return false;
	}

	this->path = outputPath;
	this->stagedLumps.clear();
	return this->readHeader();
}




*/

}
