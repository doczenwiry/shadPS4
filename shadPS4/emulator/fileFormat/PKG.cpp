#include "PKG.h"
#include "../../core/FsFile.h"

PKG::PKG()
{
}


PKG::~PKG()
{
}

bool PKG::open(const std::string& filepath) {
	FsFile file;
	if (!file.Open(filepath, fsRead))
	{
		return false;
	}
	pkgSize = file.getFileSize();
	PKGHeader pkgheader;
	file.ReadBE(pkgheader);
	//we have already checked magic should be ok

	//find title id it is part of pkg_content_id starting at offset 0x40
	file.Seek(0x47, fsSeekSet);//skip first 7 characters of content_id 
	file.Read(&pkgTitleID, sizeof(pkgTitleID));

	file.Close();

	return true;
}
bool PKG::extract(const std::string& filepath, const std::string& extractPath, std::string& failreason)
{
		this->extractPath = extractPath;
		FsFile file;
		if (!file.Open(filepath, fsRead))
		{
			return false;
		}
		pkgSize = file.getFileSize();
		PKGHeader pkgheader;
		file.ReadBE(pkgheader);

		file.Seek(0, fsSeekSet);
		pkg = (U08*)mmap(pkgSize, file.fileDescr());

		file.Read(pkg, pkgSize);
		
		U32 offset = pkgheader.pkg_table_entry_offset;
		U32 n_files = pkgheader.pkg_table_entry_count;


		for (int i = 0; i < n_files; i++) {
			PKGEntry entry = (PKGEntry&)pkg[offset + i * 0x20];
			ReadBE(entry);
			if (entry.id == 0x1200)//test code for extracting icon0
			{
				FsFile out;
				out.Open(extractPath + "icon0.png", fsWrite);
				out.Write(pkg + entry.offset, entry.size);
				out.Close();
			}
		}
		munmap(pkg);
		return true;
}