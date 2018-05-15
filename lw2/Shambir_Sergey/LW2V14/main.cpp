/**
*                       Задача о картинной галерее
* Вахтер следит за тем, чтобы в картинной галерее было не более M посетителей.
* Для обозрения представлены N картин. Посетитель ходит от картины к картине,
* и если на картину любуются более чем X посетителей, он стоит в стороне и
* ждет, пока число желающих увидеть картину не станет меньше. Посетитель может
* покинуть галерею.
*/

#include "stdafx.h"
#include "SemaphoreLock.h"
#include "SyncOut.h"
#include "Thread.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

using namespace std;
using namespace chrono;

namespace
{

constexpr unsigned kMinPictureWatchDurationMsec = 400;
constexpr unsigned kMaxPictureWatchDurationMsec = 1200;
const unsigned kDefaultVisitorCount = 12;
const unsigned kDefaultMaxWatchers = 2;

class Picture
{
public:
	explicit Picture(string name, unsigned maxWatchers)
		: m_name(move(name))
	{
		if (maxWatchers == 0)
		{
			throw logic_error("max watchers count cannot be 0");
		}
		m_watchersSemaphore = CreateAnonymousSemaphoreSafely(maxWatchers);
	}

	Picture(const Picture&) = delete;
	Picture& operator=(const Picture&) = delete;

	Picture(Picture&&) = default;
	Picture& operator=(Picture&&) = default;

	SemaphoreLock StartWatch() const
	{
		return SemaphoreLock{ m_watchersSemaphore };
	}

	const string GetName() const
	{
		return m_name;
	}

private:
	SemaphoreHandle m_watchersSemaphore;
	string m_name;
};

class PictureGallery
{
public:
	void AddPicture(string name, unsigned maxWatchers)
	{
		m_pictures.emplace_back(move(name), maxWatchers);
	}

	size_t GetPictureCount() const
	{
		return m_pictures.size();
	}

	const Picture& GetPicture(size_t index) const
	{
		return m_pictures.at(index);
	}

private:
	vector<Picture> m_pictures;
};

using PictureGalleryPtr = shared_ptr<PictureGallery>;
using PictureGalleryConstPtr = shared_ptr<const PictureGallery>;

class GalleryCustomer
{
public:
	explicit GalleryCustomer(string name, PictureGalleryConstPtr gallery)
		: m_name(move(name))
		, m_gallery(move(gallery))
		, m_engine(random_device()())
	{
	}

	void VisitGallery()
	{
		vector<size_t> route = GenerateRandomRoute(m_gallery->GetPictureCount());
		for (size_t index : route)
		{
			WatchPicture(m_gallery->GetPicture(index));
		}
		SyncOut::println(m_name, " leaves gallery");
	}

private:
	milliseconds GetRandomWatchDuration()
	{
		uniform_int_distribution<unsigned> distr{ kMinPictureWatchDurationMsec, kMaxPictureWatchDurationMsec };
		return milliseconds{ distr(m_engine) };
	}

	vector<size_t> GenerateRandomRoute(size_t pictureCount)
	{
		vector<size_t> indicies(pictureCount);
		iota(indicies.begin(), indicies.end(), 0);
		shuffle(indicies.begin(), indicies.end(), m_engine);

		return indicies;
	}

	void WatchPicture(const Picture& picture)
	{
		const auto waitStart = system_clock::now();
		SemaphoreLock lock = picture.StartWatch();
		const milliseconds waitDuration = duration_cast<milliseconds>(system_clock::now() - waitStart);

		if (waitDuration != 0ms)
		{
			SyncOut::println(m_name, " now looks at ", std::quoted(picture.GetName()), " after waiting ", waitDuration.count(), "ms in queue");
		}
		else
		{
			SyncOut::println(m_name, " now looks at ", std::quoted(picture.GetName()));
		}

		this_thread::sleep_for(GetRandomWatchDuration());
	}

	string m_name;
	PictureGalleryConstPtr m_gallery;
	vector<size_t> m_route;
	mt19937 m_engine;
};

struct CmdArguments
{
	unsigned visitorCount = 0;
	unsigned maxWatchers = 0;
};

CmdArguments ParseCmdArguments(int argc, char* argv[])
{
	CmdArguments args;
	args.visitorCount = (argc > 1) ? stoi(argv[1]) : kDefaultVisitorCount;
	args.maxWatchers = (argc > 2) ? stoi(argv[2]) : kDefaultMaxWatchers;
	return args;
}

string BuildCustomerName(unsigned index)
{
	const char* namesPool[] = {
		u8"John",
		u8"Jacob",
		u8"Michael",
		u8"Daniel",
		u8"Emma",
		u8"Hannah",
		u8"Samantha",
		u8"Olivia",
		u8"Daniel",
		u8"Robert",
		u8"Sofia",
		u8"Kamila",
		u8"Mary",
		u8"William",
		u8"Charles",
		u8"Zoe",
	};
	constexpr size_t namesPoolSize = std::size(namesPool);
	string name = namesPool[index % namesPoolSize];
	if (index >= namesPoolSize)
	{
		name += std::to_string(1 + index / namesPoolSize);
	}
	return name;
}

} // namespace

int main(int argc, char* argv[])
{
	try
	{
		const CmdArguments args = ParseCmdArguments(argc, argv);

		// Init gallery, see also https://en.wikipedia.org/wiki/Ivan_Aivazovsky#Works
		PictureGalleryPtr gallery = std::make_shared<PictureGallery>();
		gallery->AddPicture(u8"Azure Grotto, Naples", args.maxWatchers);
		gallery->AddPicture(u8"Bracing The Waves", args.maxWatchers);
		gallery->AddPicture(u8"Rainbow", args.maxWatchers);
		gallery->AddPicture(u8"Tower", args.maxWatchers);
		gallery->AddPicture(u8"Chaos", args.maxWatchers);
		gallery->AddPicture(u8"Top-Kahne Mosque", args.maxWatchers);

		std::vector<Thread> threads;
		for (unsigned index = 0; index < args.visitorCount; ++index)
		{
			threads.emplace_back([index, gallery] {
				GalleryCustomer customer{ BuildCustomerName(index), gallery };
				customer.VisitGallery();
			});
		}
		for (Thread& thread : threads)
		{
			thread.Join();
		}
	}
	catch (const exception& ex)
	{
		cerr << "FATAL ERROR: " << ex.what() << endl;
	}
}
