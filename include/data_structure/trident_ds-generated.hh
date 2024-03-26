#ifndef TRIDENT_DS_GENERATED
#define TRIDENT_DS_GENERATED
struct EventNumber;
struct RawPmtSegment;
struct RawSegment;
struct RawData;
struct GroupData;

struct EventNumber {
    uint32_t eventNumber;

    bool operator==(const EventNumber& other) const
    {
        return (eventNumber==other.eventNumber);
    }

    template <class Other>
    auto operator<(const Other& other) const
        -> typename std::enable_if<std::is_same<Other, EventNumber>::value, decltype(bool(eventNumber<other.eventNumber))>::type
    {
        if (eventNumber != other.eventNumber) return eventNumber < other.eventNumber;
        return false;
    }

    PBSS_TUPLE_MEMBERS(PBSS_TUPLE_MEMBER(&EventNumber::eventNumber));
};

struct RawPmtSegment {
    uint64_t startTime;
    uint32_t channelNumber;
    std::vector<uint16_t> adcValue;

    bool operator==(const RawPmtSegment& other) const
    {
        return (startTime==other.startTime) && (channelNumber==other.channelNumber) && (adcValue==other.adcValue);
    }

    template <class Other>
    auto operator<(const Other& other) const
        -> typename std::enable_if<std::is_same<Other, RawPmtSegment>::value, decltype(bool(startTime<other.startTime), bool(channelNumber<other.channelNumber), bool(adcValue<other.adcValue))>::type
    {
        if (startTime != other.startTime) return startTime < other.startTime;
        if (channelNumber != other.channelNumber) return channelNumber < other.channelNumber;
        if (adcValue != other.adcValue) return adcValue < other.adcValue;
        return false;
    }

    PBSS_TUPLE_MEMBERS(PBSS_TUPLE_MEMBER(&RawPmtSegment::startTime), PBSS_TUPLE_MEMBER(&RawPmtSegment::channelNumber), PBSS_TUPLE_MEMBER(&RawPmtSegment::adcValue));
};

struct RawSegment {
    uint64_t startTime;
    uint32_t channelNumber;
    std::vector<uint16_t> adcValue;
    uint64_t busyTime;
    uint32_t sampleSize;
    float baseLine;

    bool operator==(const RawSegment& other) const
    {
        return (startTime==other.startTime) && (channelNumber==other.channelNumber) && (adcValue==other.adcValue) && (busyTime==other.busyTime) && (sampleSize==other.sampleSize) && (baseLine==other.baseLine);
    }

    template <class Other>
    auto operator<(const Other& other) const
        -> typename std::enable_if<std::is_same<Other, RawSegment>::value, decltype(bool(startTime<other.startTime), bool(channelNumber<other.channelNumber), bool(adcValue<other.adcValue), bool(busyTime<other.busyTime), bool(sampleSize<other.sampleSize), bool(baseLine<other.baseLine))>::type
    {
        if (startTime != other.startTime) return startTime < other.startTime;
        if (channelNumber != other.channelNumber) return channelNumber < other.channelNumber;
        if (adcValue != other.adcValue) return adcValue < other.adcValue;
        if (busyTime != other.busyTime) return busyTime < other.busyTime;
        if (sampleSize != other.sampleSize) return sampleSize < other.sampleSize;
        if (baseLine != other.baseLine) return baseLine < other.baseLine;
        return false;
    }

    PBSS_TUPLE_MEMBERS(PBSS_TUPLE_MEMBER(&RawSegment::startTime), PBSS_TUPLE_MEMBER(&RawSegment::channelNumber), PBSS_TUPLE_MEMBER(&RawSegment::adcValue), PBSS_TUPLE_MEMBER(&RawSegment::busyTime), PBSS_TUPLE_MEMBER(&RawSegment::sampleSize), PBSS_TUPLE_MEMBER(&RawSegment::baseLine));
};

struct RawData {
    uint32_t packageId;
    uint32_t packageNumber;
    uint64_t packageTime;
    uint64_t packageEndTime;
    std::vector<RawPmtSegment> rawSegments;

    bool operator==(const RawData& other) const
    {
        return (packageId==other.packageId) && (packageNumber==other.packageNumber) && (packageTime==other.packageTime) && (packageEndTime==other.packageEndTime) && (rawSegments==other.rawSegments);
    }

    PBSS_TAGGED_STRUCT(PBSS_TAG_MEMBER(1, &RawData::packageId), PBSS_TAG_MEMBER(2, &RawData::packageNumber), PBSS_TAG_MEMBER(3, &RawData::packageTime), PBSS_TAG_MEMBER(4, &RawData::packageEndTime), PBSS_TAG_MEMBER(5, &RawData::rawSegments));
};

struct GroupData {
    uint32_t runNumber;
    uint32_t groupNumber;
    uint64_t startTime;
    uint64_t endTime;
    std::vector<RawSegment> segments;

    bool operator==(const GroupData& other) const
    {
        return (runNumber==other.runNumber) && (groupNumber==other.groupNumber) && (startTime==other.startTime) && (endTime==other.endTime) && (segments==other.segments);
    }

    PBSS_TAGGED_STRUCT(PBSS_TAG_MEMBER(1, &GroupData::runNumber), PBSS_TAG_MEMBER(2, &GroupData::groupNumber), PBSS_TAG_MEMBER(3, &GroupData::startTime), PBSS_TAG_MEMBER(4, &GroupData::endTime), PBSS_TAG_MEMBER(5, &GroupData::segments));
};

#endif
