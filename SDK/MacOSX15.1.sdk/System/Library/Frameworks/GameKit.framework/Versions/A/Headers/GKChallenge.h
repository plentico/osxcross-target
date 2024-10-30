// Copyright © Apple Inc. All rights reserved.

#import <TargetConditionals.h>
#import <Foundation/Foundation.h>
#import <GameKit/GKScore.h>
#import <GameKit/GKLeaderboardEntry.h>
#import <GameKit/GKLeaderboardScore.h>
#import <GameKit/GKAchievement.h>

NS_ASSUME_NONNULL_BEGIN

@class GKPlayer;

typedef NS_ENUM(NSInteger, GKChallengeState) {
    GKChallengeStateInvalid = 0,
    GKChallengeStatePending = 1, // The challenge has been issued, but neither completed nor declined
    GKChallengeStateCompleted = 2, // The challenge has been completed by the receiving player
    GKChallengeStateDeclined = 3, // The challenge has been declined by the receiving player
};

NS_CLASS_AVAILABLE(10_8, 6_0) __WATCHOS_PROHIBITED
@interface GKChallenge : NSObject <NSCoding, NSSecureCoding>

/// Query challenges for the current game issued to the local player -- equivalent GKChallenge objects are not guaranteed to be pointer equivalent across calls, but equal GKChallenge objects will have equal hashes
+ (void)loadReceivedChallengesWithCompletionHandler:(void(^ __nullable)(NSArray<GKChallenge *> * __nullable challenges, NSError * __nullable error))completionHandler;

/// Any GKChallenge object to be declined must be in a state of GKChallengeStatePending in order to be successfully cancelled
- (void)decline;

/// The GKPlayer who issued the challenge
@property (nonatomic, readonly, nullable, copy) GKPlayer *issuingPlayer NS_AVAILABLE(10_10, 8_0);
/// The GKPlayer who has received the challenge
@property (nonatomic, readonly, nullable, copy) GKPlayer *receivingPlayer NS_AVAILABLE(10_10, 8_0);
/// Current state of the challenge
@property (nonatomic, readonly, assign) GKChallengeState state;
/// Date the challenge was issued
@property (nonatomic, readonly, retain) NSDate *issueDate;
/// Date the challenge was completed or aborted
@property (nonatomic, readonly, nullable, retain) NSDate *completionDate;
/// The message sent to receivers of this challenge
@property (nonatomic, readonly, nullable, copy) NSString *message;


@end

__WATCHOS_PROHIBITED
@interface GKChallenge (Obsoleted)
/*** This property is obsolete. ***/
@property (nonatomic, readonly, nullable, copy) NSString *issuingPlayerID API_DEPRECATED_WITH_REPLACEMENT("-issuingPlayer:", ios(6.0,8.0), macos(10.8,10.10)) __TVOS_UNAVAILABLE;
/*** This property is obsolete. ***/
@property (nonatomic, readonly, nullable, copy) NSString *receivingPlayerID API_DEPRECATED_WITH_REPLACEMENT("-receivingPlayer:", ios(6.0,8.0), macos(10.8,10.10)) __TVOS_UNAVAILABLE;
@end

NS_CLASS_AVAILABLE(10_8, 6_0) __WATCHOS_PROHIBITED
@interface GKScoreChallenge : GKChallenge

/// The score to meet to satisfy this challenge
@property (nonatomic, readonly, nullable, retain) GKScore *score API_DEPRECATED_WITH_REPLACEMENT("-leaderboardEntry:", ios(6.0, 17.4), macos(10.8, 14.4), tvos(9.0, 17.4), visionos(1.0, 1.1));

/// The leaderboard entry to meet to satisfy this challenge
@property (nonatomic, readonly, nullable, retain) GKLeaderboardEntry *leaderboardEntry API_AVAILABLE(ios(17.4), macos(14.4), tvos(17.4));

@end

NS_CLASS_AVAILABLE(10_8, 6_0) __WATCHOS_PROHIBITED
@interface GKAchievementChallenge : GKChallenge

/// The achievement to achieve to satisfy this challenge
@property (nonatomic, readonly, nullable, retain) GKAchievement *achievement;
@end


#if !TARGET_OS_WATCH

/// Use the following category methods to issue GKScoreChallenges and GKAchievementChallenges to an array of playerIDs. Players may not issue challenges to themselves nor to non-friends. Please see the GameKit reference documentation for further details on these methods.
@interface GKScore (GKChallenge)

/// Use this alternative to reportScores:withCompletionHandler: to allow only certain specific challenges to be completed. Pass nil to avoid completing any challenges.
+ (void)reportScores:(NSArray<GKScore *> *)scores withEligibleChallenges:(NSArray<GKChallenge *> *)challenges withCompletionHandler:(void(^ __nullable)(NSError * __nullable error))completionHandler API_DEPRECATED_WITH_REPLACEMENT("-reportLeaderboardScores:withEligibleChallenges:withCompletionHandler", ios(6.0,14.0), macos(10.10,11.0));

+ (void)reportLeaderboardScores:(NSArray<GKLeaderboardScore *> *)scores withEligibleChallenges:(NSArray<GKChallenge *> *)challenges withCompletionHandler:(void(^ __nullable)(NSError * __nullable error))completionHandler NS_AVAILABLE(11_0, 14_0);

@end

@interface GKAchievement (GKChallenge)

/// Given a list of players, return a subset of that list containing only players that are eligible to receive a challenge for the achievement.
- (void)selectChallengeablePlayers:(NSArray<GKPlayer *> *)players withCompletionHandler:(void(^ __nullable)(NSArray<GKPlayer *> * __nullable challengeablePlayers, NSError * __nullable error))completionHandler NS_AVAILABLE(10_10, 8_0);

/// Use this alternative to reportAchievements:withCompletionHandler: to allow only certain specific challenges to be completed. Pass nil to avoid completing any challenges.
+ (void)reportAchievements:(NSArray<GKAchievement *> *)achievements withEligibleChallenges:(NSArray<GKChallenge *> *)challenges withCompletionHandler:(void(^ __nullable)(NSError * __nullable error))completionHandler NS_AVAILABLE(10_10, 7_0);

@end

@interface GKScore (GKChallengeObsoleted)
/*** This method is obsolete. It will never be invoked and its implementation does nothing***/
- (void)issueChallengeToPlayers:(nullable NSArray<NSString *> *)playerIDs message:(nullable NSString *)message API_DEPRECATED_WITH_REPLACEMENT("-challengeComposeControllerWithMessage:players:completionHandler:", ios(6.0,7.0), macos(10.8,10.10)) __TVOS_UNAVAILABLE;
@end

@interface GKAchievement (GKChallengeObsoleted)
/*** This method is obsolete. It will never be invoked and its implementation does nothing***/
- (void)issueChallengeToPlayers:(nullable NSArray<NSString *> *)playerIDs message:(nullable NSString *)message API_DEPRECATED_WITH_REPLACEMENT("-challengeComposeControllerWithMessage:players:completionHandler:", ios(6.0,7.0), macos(10.8,10.10)) __TVOS_UNAVAILABLE;

/*** This method is obsolete. It will never be invoked and its implementation does nothing***/
- (void)selectChallengeablePlayerIDs:(nullable NSArray<NSString *> *)playerIDs withCompletionHandler:(void(^__nullable)(NSArray<NSString *> * __nullable challengeablePlayerIDs, NSError * __nullable error))completionHandler API_DEPRECATED_WITH_REPLACEMENT("-selectChallengeablePlayers:", ios(6.0,8.0), macos(10.8,10.10)) __TVOS_UNAVAILABLE;
@end

#if TARGET_OS_IPHONE
@class UIViewController;
typedef void (^GKChallengeComposeCompletionBlock)(UIViewController *composeController, BOOL didIssueChallenge, NSArray<NSString *> * __nullable sentPlayerIDs) API_DEPRECATED_WITH_REPLACEMENT("GKChallengeComposeHandler", ios(8.0, 17.0), tvos(9.0, 17.0));
typedef void (^GKChallengeComposeHandler)(UIViewController *composeController, BOOL didIssueChallenge, NSArray<GKPlayer *> * __nullable sentPlayers) API_AVAILABLE(ios(17.0), tvos(17.0));
#else
@class NSViewController;
typedef void (^GKChallengeComposeCompletionBlock)(NSViewController *composeController, BOOL didIssueChallenge, NSArray<NSString *> * __nullable sentPlayerIDs) API_DEPRECATED_WITH_REPLACEMENT("GKChallengeComposeHandler", macos(10.10, 14.0));
typedef void (^GKChallengeComposeHandler)(NSViewController *composeController, BOOL didIssueChallenge, NSArray<GKPlayer *> * __nullable sentPlayers) API_AVAILABLE(macos(14.0));
#endif



// Use the following category methods to issue GKScoreChallenges and GKAchievementChallenges to an array of playerIDs. Players may not issue challenges to themselves nor to non-friends. Please see the GameKit reference documentation for further details on these methods.
@interface GKScore (GKChallengeUI)

// Return a challenge compose view controller with pre-selected GKPlayers and a preformatted, player-editable message. Once this view controller is displayed, and the player sends or cancels sending the challenge, the completion handler will be called. This block contains the view controller, the reason why the handler was called, as well as which (if any) GKPlayers the challenge was sent to. Present modally from the top view controller. The completion handler should dismiss the view controller.
#if TARGET_OS_IPHONE
- (UIViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(nullable NSArray<GKPlayer *> *)players completionHandler:(nullable GKChallengeComposeCompletionBlock)completionHandler API_DEPRECATED_WITH_REPLACEMENT("-challengeComposeControllerWithMessage:players:completion:", ios(8.0, 17.0), tvos(9.0, 17.0));
- (UIViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(nullable NSArray<GKPlayer *> *)players completion:(nullable GKChallengeComposeHandler)completionHandler API_AVAILABLE(ios(17.0), tvos(17.0));
#else
- (NSViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(nullable NSArray<GKPlayer *> *)players completionHandler:(nullable GKChallengeComposeCompletionBlock)completionHandler API_DEPRECATED_WITH_REPLACEMENT("-challengeComposeControllerWithMessage:players:completion:", macos(10.10, 14.0));
- (NSViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(nullable NSArray<GKPlayer *> *)players completion:(nullable GKChallengeComposeHandler)completionHandler API_AVAILABLE(macos(14.0));
#endif

@end

@interface GKLeaderboardEntry (GKChallengeUI)

// Return a challenge compose view controller with pre-selected GKPlayers and a preformatted, player-editable message. Once this view controller is displayed, and the player sends or cancels sending the challenge, the completion handler will be called. This block contains the view controller, the reason why the handler was called, as well as which (if any) GKPlayers the challenge was sent to. Present modally from the top view controller. The completion handler should dismiss the view controller.
#if TARGET_OS_IPHONE
- (UIViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(nullable NSArray<GKPlayer *> *)players completionHandler:(nullable GKChallengeComposeCompletionBlock)completionHandler API_DEPRECATED_WITH_REPLACEMENT("-challengeComposeControllerWithMessage:players:completion:", ios(14.0, 17.0), tvos(14.0, 17.0));
- (UIViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(nullable NSArray<GKPlayer *> *)players completion:(nullable GKChallengeComposeHandler)completionHandler API_AVAILABLE(ios(17.0), tvos(17.0));
#else
- (NSViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(nullable NSArray<GKPlayer *> *)players completionHandler:(nullable GKChallengeComposeCompletionBlock)completionHandler API_DEPRECATED_WITH_REPLACEMENT("-challengeComposeControllerWithMessage:players:completion:", macos(11.0, 14.0));
- (NSViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(nullable NSArray<GKPlayer *> *)players completion:(nullable GKChallengeComposeHandler)completionHandler API_AVAILABLE(macos(14.0));
#endif

@end

@interface GKAchievement (GKChallengeUI)

// If an achievement has already been earned by the receiving player, and that achievement is not marked as replayable in App Store connect, then the challenge will not be issued to the player. If an achievement is hidden, then it will not be issued.

// Return a challenge compose view controller with pre-selected GKPlayers and a preformatted, player-editable message. Once this view controller is displayed, and the player sends or cancels sending the challenge, the completion handler will be called. This block contains the view controller, the reason why the handler was called, as well as which (if any) GKPlayers the challenge was sent to. Present modally from the top view controller. The completion handler should dismiss the view controller.
#if TARGET_OS_IPHONE
- (UIViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(NSArray<GKPlayer *> *)players completionHandler:(__nullable GKChallengeComposeCompletionBlock)completionHandler API_DEPRECATED_WITH_REPLACEMENT("-challengeComposeControllerWithMessage:players:completion:", ios(8.0, 17.0), tvos(9.0, 17.0));
- (UIViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(NSArray<GKPlayer *> *)players completion:(__nullable GKChallengeComposeHandler)completionHandler API_AVAILABLE(ios(17.0), tvos(17.0));
#else
- (NSViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(NSArray<GKPlayer *> *)players completionHandler:(__nullable GKChallengeComposeCompletionBlock)completionHandler API_DEPRECATED_WITH_REPLACEMENT("-challengeComposeControllerWithMessage:players:completion:", macos(10.10, 14.0));
- (NSViewController *)challengeComposeControllerWithMessage:(nullable NSString *)message players:(NSArray<GKPlayer *> *)players completion:(__nullable GKChallengeComposeHandler)completionHandler API_AVAILABLE(macos(14.0));
#endif

@end

@interface GKScore (GKChallengeObsoletedUI)

#if TARGET_OS_IPHONE
/*** This method is obsolete. Calling this method does nothing and will return nil ***/
- (nullable UIViewController *)challengeComposeControllerWithPlayers:(nullable NSArray<NSString *> *)playerIDs message:(nullable NSString *)message completionHandler:(__nullable GKChallengeComposeCompletionBlock)completionHandler API_DEPRECATED_WITH_REPLACEMENT("-challengeComposeControllerWithMessage:players:completion:", ios(7.0, 8.0), tvos(9.0, 17.0));
#endif

@end

@interface GKAchievement (GKChallengeObsoletedUI)

#if TARGET_OS_IPHONE
/*** This method is obsolete. Calling this method does nothing and will return nil ***/
- (nullable UIViewController *)challengeComposeControllerWithPlayers:(nullable NSArray<NSString *> *)playerIDs message:(nullable NSString *)message completionHandler:(nullable GKChallengeComposeCompletionBlock)completionHandler API_DEPRECATED_WITH_REPLACEMENT("-challengeComposeControllerWithMessage:players:completion:", ios(7.0, 8.0), tvos(9.0, 17.0));
#endif

@end

#endif // TARGET_OS_WATCH

NS_ASSUME_NONNULL_END
