//
// Created by Mihail Gutan on 12/4/15.
//
#include "WalletCallbacks.h"
#include "BRInt.h"
#include "BRTransaction.h"
#include "BRWallet.h"
#include <android/log.h>
#include "wallet.h"
#include "BRPeerManager.h"
//void Java_com_breadwallet_wallet_BRWalletManager_setCallbacks(JNIEnv *env,
//                                                              jobject thiz,
//                                                              jbyteArray walletBuff) {
//    globalEnv = env;
//    jbyte *byteWallet = (*env)->GetByteArrayElements(env, walletBuff, 0);
//    BRWallet *wallet = (BRWallet *) byteWallet;
//    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "setCallbacks");
//    //set the Wallet callbacks
//    BRWalletSetCallbacks(wallet, NULL, balanceChanged, txAdded, txUpdated, txDeleted);
//
//    int a = 10;
//    int balance = 480000;
////    balanceChanged(a, balance);
//}

//TODO TESTING ONLY, SHOULD BE REMOVED
void Java_com_breadwallet_wallet_BRWalletManager_testWalletCallbacks(JNIEnv *env,
                                                                     jobject thiz) {
//    BRWallet *w;
//    int walletLength = (*env)->GetArrayLength(env, wallet);
//    jbyte *byteWallet = (*env)->GetByteArrayElements(env, wallet, 0);
//    w = (BRWallet*) byteWallet;
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "testWalletCallbacks 1");
    const UInt256 secret = uint256_hex_decode("0000000000000000000000000000000000000000000000000000000000000001");
    BRKey k;
    BRAddress addr, recvAddr = BRWalletReceiveAddress(wallet);
    BRTransaction *tx;
    int r = 1;
    BRKeySetSecret(&k, &secret, 1);
    BRKeyAddress(&k, addr.s, sizeof(addr));

    tx = BRWalletCreateTransaction(wallet, 0, addr.s);
    if (tx) r = 0;

    tx = BRWalletCreateTransaction(wallet, SATOSHIS, addr.s);
    if (tx) r = 0;

    uint8_t inScript[BRAddressScriptPubKey(NULL, 0, addr.s)];
    size_t inScriptLen = BRAddressScriptPubKey(inScript, sizeof(inScript), addr.s);
    uint8_t outScript[BRAddressScriptPubKey(NULL, 0, recvAddr.s)];
    size_t outScriptLen = BRAddressScriptPubKey(outScript, sizeof(outScript), recvAddr.s);

    tx = BRTransactionNew();
    BRTransactionAddInput(tx, UINT256_ZERO, 0, inScript, inScriptLen, NULL, 0, TXIN_SEQUENCE);
    BRTransactionAddOutput(tx, SATOSHIS, outScript, outScriptLen);
    BRWalletRegisterTransaction(wallet, tx); // test adding unsigned tx
    if (BRWalletBalance(wallet) != 0) r = 0;

    BRTransactionSign(tx, &k, 1);
    BRWalletRegisterTransaction(wallet, tx);
    if (BRWalletBalance(wallet) != SATOSHIS) r = 0;

    tx = BRWalletCreateTransaction(wallet, SATOSHIS*2, addr.s);
    if (tx) r = 0;

    tx = BRWalletCreateTransaction(wallet, SATOSHIS/2, addr.s);
    if (! tx) r = 0;

    if (tx) BRWalletSignTransaction(wallet, tx, NULL);
    if (tx && ! BRTransactionIsSigned(tx)) r = 0;

    if (tx) BRWalletRegisterTransaction(wallet, tx);
    if (tx && BRWalletBalance(wallet) + BRWalletFeeForTx(wallet, tx) != SATOSHIS/2) r = 0;
    printf("\n");

    if (tx) BRTransactionFree(tx);

}

void Java_com_breadwallet_wallet_BRWalletManager_testTransactionAdding(JNIEnv *env,
                                                                     jobject thiz) {
//    BRWallet *w;
//    int walletLength = (*env)->GetArrayLength(env, wallet);
//    jbyte *byteWallet = (*env)->GetByteArrayElements(env, wallet, 0);
//    w = (BRWallet*) byteWallet;
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", ">>>>>> testTransactionAdding >>>>>>>>>>");
    int r = 1;
    const UInt256 secret = uint256_hex_decode("0000000000000000000000000000000000000000000000000000000000000101");
    BRKey k;
    BRAddress addr, recvAddr = BRWalletReceiveAddress(wallet);
    BRTransaction *tx;
    BRKeySetSecret(&k, &secret, 1);
    BRKeyAddress(&k, addr.s, sizeof(addr));

    tx = BRWalletCreateTransaction(wallet, 0, addr.s);
    if (tx) r = 0;
    tx = BRWalletCreateTransaction(wallet, 43252352, addr.s);
    if (tx) r = 0;
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", ">>>>>> testTransactionAdding >>>>>>>>>> 2");
    uint8_t inScript[BRAddressScriptPubKey(NULL, 0, addr.s)];
    size_t inScriptLen = BRAddressScriptPubKey(inScript, sizeof(inScript), addr.s);
    uint8_t outScript[BRAddressScriptPubKey(NULL, 0, recvAddr.s)];
    size_t outScriptLen = BRAddressScriptPubKey(outScript, sizeof(outScript), recvAddr.s);
    tx = BRTransactionNew();
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", ">>>>>> testTransactionAdding >>>>>>>>>> 3");
    BRTransactionAddInput(tx, UINT256_ZERO, 0, inScript, inScriptLen, NULL, 0, TXIN_SEQUENCE);
    BRTransactionAddOutput(tx, 43252352, outScript, outScriptLen);
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", ">>>>>> testTransactionAdding >>>>>>>>>> 4");
    BRTransactionSign(tx, &k, 1);
    BRWalletRegisterTransaction(wallet, tx);
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", ">>>>>> testTransactionAdding >>>>>>>>>> 5");
    tx = BRWalletCreateTransaction(wallet, 756756, addr.s);
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", ">>>>>> testTransactionAdding >>>>>>>>>> 6");
    tx = BRWalletCreateTransaction(wallet, 3453534, addr.s);
    if (tx) BRWalletSignTransaction(wallet, tx, NULL);
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", ">>>>>> testTransactionAdding >>>>>>>>>> 7");
    if (tx && ! BRTransactionIsSigned(tx)) r = 0;
    if (tx) BRWalletRegisterTransaction(wallet, tx);
    if (tx && BRWalletBalance(wallet) + BRWalletFeeForTx(wallet, tx) != SATOSHIS/2) r = 0;
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", ">>>>>> testTransactionAdding >>>>>>>>>> 8");
    printf("\n");
    if (tx) BRTransactionFree(tx);

}