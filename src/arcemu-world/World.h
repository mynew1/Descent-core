/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __WORLD_H
#define __WORLD_H

#define IS_INSTANCE(a) (((a)>1)&&((a)!=530)&&((a)!=571))

class Object;
class WorldPacket;
class WorldSession;
class Unit;
class Creature;
class GameObject;
class DynamicObject;
class Player;
class EventableObjectHolder;
class MapMgr;
class Battleground;

enum Rates
{
	RATE_HEALTH=0,
	RATE_POWER1,	//mana
	RATE_POWER2,	//rage
	RATE_POWER3,	//focus
	RATE_POWER4,	//energy
	RATE_POWER5,	//runic
	RATE_DROP0, // separate rates for each quality level
	RATE_DROP1,
	RATE_DROP2,
	RATE_DROP3,
	RATE_DROP4,
	RATE_DROP5,
	RATE_DROP6,
	RATE_MONEY,
	RATE_XP,
	RATE_RESTXP,
	RATE_QUESTXP,
	RATE_EXPLOREXP,
	RATE_WEEKENDXP,
	RATE_HONOR,
	RATE_QUESTREPUTATION,
	RATE_KILLREPUTATION,
	RATE_SKILLCHANCE,
	RATE_SKILLRATE,
	RATE_ARENAPOINTMULTIPLIER2X,
	RATE_ARENAPOINTMULTIPLIER3X,
	RATE_ARENAPOINTMULTIPLIER5X,
	RATE_MOVE_SPEED,
	RATE_CAST_AND_COOLDOWN_SPEED,
	MAX_RATES
};		

enum IntRates
{
	INTRATE_SAVE=0,
	INTRATE_COMPRESSION,
	INTRATE_PVPTIMER,
	INTRATE_START_EXTRA_TP,
	INTRATE_LOGIN_BUFF1,
	INTRATE_RESPAWN_COOLDOWN,
	INTRATE_RANDOMIZEITEMS1,
	INTRATE_DISABLEITEMCONSUME,
	INTRATE_ALLOW_QUEST_TALENTPOINTS,
	INTRATE_NO_CURRENCY_LIMITS,
	INTRATE_DISABLE_MAIL_FEATURE,
	INTRATE_DISABLE_TRADE_FEATURE,
	INTRATE_LOGON_TYPE,		// 1 - try Arcemu, 2 - try Trinity
	MAX_INTRATES
};


enum EnviromentalDamage
{
	DAMAGE_EXHAUSTED = 0,
	DAMAGE_DROWNING = 1,
	DAMAGE_FALL = 2,
	DAMAGE_LAVA = 3,
	DAMAGE_SLIME = 4,
	DAMAGE_FIRE = 5
};


// ServerMessages.dbc
enum ServerMessageType
{
	SERVER_MSG_SHUTDOWN_TIME			= 1,
	SERVER_MSG_RESTART_TIME				= 2,
	SERVER_MSG_STRING					= 3,
	SERVER_MSG_SHUTDOWN_CANCELLED		= 4,
	SERVER_MSG_RESTART_CANCELLED		= 5,
	SERVER_MSG_BATTLEGROUND_SHUTDOWN	= 6,
	SERVER_MSG_BATTLEGROUND_RESTART		= 7,
	SERVER_MSG_INSTANCE_SHUTDOWN		= 8,
	SERVER_MSG_INSTANCE_RESTART			= 9
};

enum WorldMapInfoFlag
{
	WMI_INSTANCE_ENABLED   = 0x1,
	WMI_INSTANCE_WELCOME   = 0x2,
	WMI_INSTANCE_MULTIMODE = 0x4,
	WMI_INSTANCE_XPACK_01  = 0x8, //The Burning Crusade expansion
	WMI_INSTANCE_XPACK_02  = 0x10, //wotlk
	WMI_INSTANCE_XPACK_03  = 0x20, //cata (need to check)
	WMI_INSTANCE_XPACK_04  = 0x40, //pandaria (need to check)
};

enum AccountFlags
{
	ACCOUNT_FLAG_VIP		 = 0x1,
	ACCOUNT_FLAG_NO_AUTOJOIN = 0x2,
	//ACCOUNT_FLAG_XTEND_INFO  = 0x4,
	ACCOUNT_FLAG_XPACK_01	= 0x8,
	ACCOUNT_FLAG_XPACK_02	= 0x10,	//will change on wotlk
	ACCOUNT_FLAG_XPACK_03	= 0x20,	//will change on cata
	ACCOUNT_FLAG_XPACK_04	= 0x40,	//will change on mists of pandaria. Not confirmed !
};

#pragma pack(push,1)
struct MapInfo
{
	uint32 mapid;
	uint32 screenid;
	uint32 type_flags;
	uint32 playerlimit;
	uint32 minlevel;
	float repopx;
	float repopy;
	float repopz;
	uint32 repopmapid;
	char * name;
	uint32 flags;
	uint32 cooldown;
    uint32 lvl_mod_a;
	uint32 required_quest;
	uint32 required_item;
	uint32 heroic_key_1;
	uint32 heroic_key_2;
	float update_distance;
	uint32 checkpoint_id;
	uint32 required_avg_item_level;
	uint32 required_heroic_avg_item_level;

	bool HasFlag(uint32 flag)
	{
		return (flags & flag) != 0;
	}
};

#pragma pack(pop)

enum REALM_TYPE
{
    REALM_PVE = 0,
    REALM_PVP = 1,
};
struct AreaTable;

class BasicTaskExecutor : public ThreadBase
{
	CallbackBase * cb;
	uint32 priority;
public:
	BasicTaskExecutor(CallbackBase * Callback, uint32 Priority) : cb(Callback), priority(Priority) {}
	~BasicTaskExecutor() 
	{ 
		delete cb; 
		cb = NULL;
	}
	bool run();
};

class Task
{
	CallbackBase * _cb;
public:
	Task(CallbackBase * cb) : _cb(cb), completed(false), in_progress(false) {}
	~Task() 
	{ 
		delete _cb; 
		_cb = NULL;
	}
	bool completed;
	bool in_progress;
	void execute();
};

struct WMOAreaTableTripple
{
	WMOAreaTableTripple(int32 r, int32 a, int32 g) : groupId(g), rootId(r), adtId(a)
	{
	}

	bool operator <(const WMOAreaTableTripple & b) const
	{
		return memcmp(this, &b, sizeof(WMOAreaTableTripple)) < 0;
	}

	// ordered by entropy; that way memcmp will have a minimal medium runtime
	int32 groupId;
	int32 rootId;
	int32 adtId;
};

struct CharacterLoaderThread : public ThreadBase
{
#if (defined( WIN32 ) || defined( WIN64 ) )
	HANDLE hEvent;
#else
	pthread_cond_t cond;
	pthread_mutex_t mutex;
#endif
	bool running;
public:
	CharacterLoaderThread();
	~CharacterLoaderThread();
	void OnShutdown();
	bool run();
};

class TaskList
{
	set<Task*> tasks;
	Mutex queueLock;
public:
	Task * GetTask();
	void AddTask(Task* task);
	void RemoveTask(Task * task)
	{
		queueLock.Acquire();
		tasks.erase(task);
		queueLock.Release();
	}

	void spawn();
	void kill();

	void wait();
	void waitForThreadsToExit();
	uint32 thread_count;
	bool running;

	Mutex tcMutex;
	void incrementThreadCount()
	{
		tcMutex.Acquire();
		++thread_count;
		tcMutex.Release();
	}

	void decrementThreadCount()
	{
		tcMutex.Acquire();
		--thread_count;
		tcMutex.Release();
	}
};

enum BasicTaskExecutorPriorities
{
	BTE_PRIORITY_LOW		= 0,
	BTE_PRIORITY_MED		= 1,
	BTW_PRIORITY_HIGH	   = 2,
};

class TaskExecutor : public ThreadBase
{
	TaskList * starter;
public:
	TaskExecutor(TaskList * l) : starter(l) { l->incrementThreadCount(); }
	~TaskExecutor() { starter->decrementThreadCount(); }

	bool run();
};

class WorldSocket;

// Slow for remove in middle, oh well, wont get done much.
typedef std::list<WorldSocket*> QueueSet;
typedef set<WorldSession*> SessionSet;

class SERVER_DECL World : public Singleton<World>, public EventableObject
{
public:
	World();
	~World();

	/** Reloads the config and sets all of the setting variables 
	 */
	void Rehash(bool load);

	void CleanupCheaters();
	WorldSession* FindSession(uint32 id);
	WorldSession* FindSessionByName(const char *);

	void MuteSessionsByIP(const char *,uint32 end_stamp);
	void UnMuteSessionsByIP(const char *);

	void AddSession(WorldSession *s);
	void RemoveSession(uint32 id);

	void AddGlobalSession(WorldSession *session);
	void RemoveGlobalSession(WorldSession *session);
	void DeleteSession(WorldSession *session);

	ARCEMU_INLINE size_t GetSessionCount() const { return m_sessions.size(); }
	uint32 GetNonGmSessionCount();
	ARCEMU_INLINE size_t GetQueueCount() { return mQueuedSessions.size(); }
	void GetStats(uint32 * GMCount, float * AverageLatency);

	ARCEMU_INLINE uint32 GetPlayerLimit() const { return m_playerLimit; }
	void SetPlayerLimit(uint32 limit) { m_playerLimit = limit; }

	ARCEMU_INLINE bool getAllowMovement() const { return m_allowMovement; }
	void SetAllowMovement(bool allow) { m_allowMovement = allow; }
	ARCEMU_INLINE bool getGMTicketStatus() { return m_gmTicketSystem; };
	bool toggleGMTicketStatus()
	{
		m_gmTicketSystem = !m_gmTicketSystem;
		return m_gmTicketSystem;
	};

	ARCEMU_INLINE std::string getGmClientChannel() { return GmClientChannel; }

	void SetMotd(const char *motd) { m_motd = motd; }
	ARCEMU_INLINE const char* GetMotd() const { return m_motd.c_str(); }

	ARCEMU_INLINE time_t GetGameTime() const { return m_gameTime; }

	bool SetInitialWorldSettings();

	void SendWorldText(const char *text, WorldSession *self = 0);
	void SendWorldWideScreenText(const char *text, WorldSession *self = 0);
	void SendGlobalMessage(WorldPacket *packet, WorldSession *self = 0);
	void SendZoneMessage(WorldPacket *packet, uint32 zoneid, WorldSession *self = 0);
	void SendInstanceMessage(WorldPacket *packet, uint32 instanceid, WorldSession *self = 0);
	void SendFactionMessage(WorldPacket *packet, uint8 teamId);
	void SendGamemasterMessage(WorldPacket *packet, WorldSession *self = 0);
	void SendGMWorldText(const char* text, WorldSession *self = 0);

	ARCEMU_INLINE void SetStartTime(uint32 val) { m_StartTime = val; }
	ARCEMU_INLINE uint32 GetUptime(void) { return (uint32)UNIXTIME - m_StartTime; }
	ARCEMU_INLINE uint32 GetStartTime(void) { return m_StartTime; }
	std::string GetUptimeString();

	// update the world server every frame
	void Update(time_t diff);
	void CheckForExpiredInstances();

   
	void UpdateSessions(uint32 diff);

	ARCEMU_INLINE void setRate(int index,float value)
	{
		regen_values[index]=value;
	}

	ARCEMU_INLINE float getRate(int index)
	{
		return regen_values[index];
	}
	
	ARCEMU_INLINE int32 getIntRate(int index)
	{
		return int_rates[index];
	}

	ARCEMU_INLINE void setIntRate(int index, int32 value)
	{
		int_rates[index] = value;
	}

	ARCEMU_INLINE uint32 GetTimeOut(){return TimeOut;}

	struct NameGenData
	{
		string name;
		uint32 type;
	};
	vector<NameGenData> _namegendata[3];
	void LoadNameGenData();
	std::string GenerateName(uint32 type = 0);

	uint32 AddQueuedSocket(WorldSocket* Socket);
	void RemoveQueuedSocket(WorldSocket* Socket);
	uint32 GetQueuePos(WorldSocket* Socket);
	void UpdateQueuedSessions(uint32 diff);

	Mutex queueMutex;

	uint32 mQueueUpdateInterval;
	bool m_useIrc;

	void SaveAllPlayers();

	string MapPath;
	string vMapPath;
	bool UnloadMapFiles;
	bool BreathingEnabled;
	bool SpeedhackProtection;
	uint32 mInWorldPlayerCount;
	uint32 mAcceptedConnections;
	uint32 SocketSendBufSize;
	uint32 SocketRecvBufSize;

	uint32 HordePlayers;
	uint32 AlliancePlayers;
	uint32 PeakSessionCount;
	uint32 ArenaQueueDiff;
	bool SendStatsOnJoin;
	SessionSet gmList;
	bool Collision;

	void ShutdownClasses();

	uint32 compression_threshold;

	void	SetKickAFKPlayerTime(uint32 idletimer){m_KickAFKPlayers=idletimer;}
	uint32	GetKickAFKPlayerTime(){return m_KickAFKPlayers;}

    uint32 GetRealmType() { return realmtype; }

	uint32 flood_lines;
	uint32 flood_seconds_ban;
	uint32 flood_seconds;
	bool flood_message;
	bool gm_skip_attunement;

	bool show_gm_in_who_list;
	uint32 map_unload_time;

	bool interfaction_chat;
	bool interfaction_group;
	bool interfaction_guild;
	bool interfaction_trade;
	bool interfaction_friend;
	bool interfaction_misc;
	bool crossover_chars;
	bool antiMasterLootNinja;
	bool gamemaster_listOnlyActiveGMs;
	bool gamemaster_hidePermissions;

	bool realmAllowTBCcharacters;
	
	std::string announce_tag;
	bool GMAdminTag;
	bool NameinAnnounce;
	bool NameinWAnnounce;
	bool announce_output;

	int announce_tagcolor;
	int announce_gmtagcolor;
	int announce_namecolor;
	int announce_msgcolor;
	string ann_namecolor;
	string ann_gmtagcolor;
	string ann_tagcolor;
	string ann_msgcolor;
	void AnnounceColorChooser(int tagcolor, int gmtagcolor, int namecolor, int msgcolor);

	int start_level;
	bool antihack_teleport;
	bool antihack_speed;
	bool antihack_flight;
	uint32 flyhack_threshold;
	bool no_antihack_on_gm;

	bool instance_TakeGroupLeaderID;
	bool instance_SlidingExpiration;
	int instance_DailyHeroicInstanceResetHour;

	int GMTTimeZone;

	void CharacterEnumProc(QueryResultVector& results, uint32 AccountId);
	void LoadAccountDataProc(QueryResultVector& results, uint32 AccountId);

	void PollCharacterInsertQueue(DatabaseConnection * con);
	void PollMailboxInsertQueue(DatabaseConnection * con);
	void DisconnectUsersWithAccount(const char * account, WorldSession * session);
	void DisconnectUsersWithIP(const char * ip, WorldSession * session);
	void DisconnectUsersWithPlayerName(const char * plr, WorldSession * session);

	void LogoutPlayers();

protected:
	// update Stuff, FIXME: use diff
	time_t _UpdateGameTime()
	{
		// Update Server time
		time_t thisTime = UNIXTIME;
		m_gameTime += thisTime - m_lastTick;
		m_gameTime = m_gameTime % (60*60*24);
		m_lastTick = thisTime;

		return m_gameTime;
	}
	void FillSpellReplacementsTable();

private:
	EventableObjectHolder * eventholder;
	//! Timers
	typedef HM_NAMESPACE::hash_map<uint32, WorldSession*> SessionMap;
	SessionMap m_sessions;
	RWLock m_sessionlock;

	typedef HM_NAMESPACE::hash_map<uint32, AreaTrigger*> AreaTriggerMap;
	AreaTriggerMap m_AreaTrigger;

protected:
	Mutex SessionsMutex;//FOR GLOBAL !
	SessionSet Sessions;

	float regen_values[MAX_RATES];
	int32 int_rates[MAX_INTRATES];

	uint32 m_playerLimit;
	bool m_allowMovement;
	bool m_gmTicketSystem;
	std::string m_motd;
   
    uint32 realmtype;

	time_t m_gameTime;
	time_t m_lastTick;
	uint32 TimeOut;

	uint32 m_StartTime;
	uint32 m_queueUpdateTimer;

	QueueSet mQueuedSessions;

	uint32	m_KickAFKPlayers;//don't lag the server if you are useless anyway :P
public:
	// static update masks
	UpdateMask	*create_update_for_other_masks[TYPEID_UNUSED];
	void GenerateStaticUpdateMasks();
	std::string GmClientChannel;
	bool m_reqGmForCommands;
	bool m_lfgForNonLfg;
//	list<SpellEntry*> dummyspells;
	uint32 m_levelCap;
	uint32 m_genLevelCap;
	bool m_limitedNames;
	bool m_useAccountData;

	char * m_banTable;

	float	mob_NormalHealSpellFactor;
	float	mob_NormalDmgSpellFactor;
	float	mob_HeroicHealSpellFactor;
	float	mob_HeroicDmgSpellFactor;
	float	mob_HeroicSpellCostPerLVL;
	float	mob_HeroicHPEXPPerLVL;
	float	mob_HeroicArmorEXPPerLVL;
	float	mob_HeroicArmorFlatPerLVL;
	float	mob_HeroicResFlatPerLVL;
	float	mob_HeroicSpellResEXPPerLVL;
	float	mob_HeroicAttackFlatSpeedPerLVL;
	float	mob_HeroicDMGEXPPerLVL;
	float	mob_HeroicSpellCooldownPCT;
	float	mob_SpellKillerBlowBelow;
	float	mob_SpellKillerBlowReduction;
	float	AnyKillerBlowBelow;
	float	AnyKillerBlowReduction;

	uint32	GoldStartAmount;

	float	AlianceExtraXPRate;		//there is a global and a team based XP rate
	float	HordeExtraXPRate;		//there is a global and a team based XP rate

	uint32	MaxLootListSize;		

	//from 3.1.x blizz started using GUIDS for packets too. They are used for serialization on a laggy network or cause they had crappy coders
	uint32	Global_Pet_id;			//This is used by client to identify a pet name and combat log details. Like a sub guid. Must not be 0

	uint32	ClientCacheVersion;		//maybe it forces client to flush client cache

	void LoadWMOAreaData()
	{
		for(uint32 i=0; i < dbcWMOAreaTable.GetNumRows(); i++)
		{
			WMOAreaTableEntry *e = dbcWMOAreaTable.LookupRow( i );
			WMOAreaTableTripple tmp(e->rootId, e->adtId, e->groupId);
			m_WMOAreaTableTripples.insert(std::make_pair(tmp, e));
		}
	}
	WMOAreaTableEntry* GetWMOAreaData(int32 rootid, int32 adtid, int32 groupid)
	{
		WMOAreaTableTripple tmp(rootid, adtid, groupid);
		std::map<WMOAreaTableTripple, WMOAreaTableEntry*>::iterator itr = m_WMOAreaTableTripples.find(tmp);

		if(itr != m_WMOAreaTableTripples.end())
			return itr->second;
		return NULL;
	}
	std::map<WMOAreaTableTripple, WMOAreaTableEntry*> m_WMOAreaTableTripples;
};

#define sWorld World::getSingleton()

#endif
