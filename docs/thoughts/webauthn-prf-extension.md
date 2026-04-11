# WebAuthn PRF Extension - Tài liệu Chi tiết

## 📖 Tổng quan

**PRF (Pseudo-Random Function)** là một extension của WebAuthn cho phép tạo ra **secret deterministic** từ passkey mà không cần lộ private key WebAuthn ra ngoài. Đây là giải pháp tối ưu để wrap/encrypt crypto keys một cách an toàn trong môi trường serverless.

---

## 🔑 Khái niệm cơ bản

### WebAuthn Passkey

Mỗi passkey bao gồm:
- **Public Key**: Có thể chia sẻ, dùng để verify signatures
- **Private Key** (`sk_webauthn`): **LOCKED** trong authenticator (TPM, Secure Enclave, hardware token)
- **Credential ID**: Identifier duy nhất của passkey

```
┌─────────────────────────────────────┐
│      Authenticator (Hardware)       │
│  ┌───────────────────────────────┐  │
│  │   Passkey                     │  │
│  │   - credential_id: "abc123"   │  │
│  │   - sk_webauthn: [LOCKED]     │  │ ← NEVER leaves device
│  │   - public_key: "xyz789"      │  │ ← Can be shared
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

---

## 🎯 PRF Extension hoạt động như thế nào?

### Công thức toán học

```
secret = HMAC-SHA256(sk_webauthn, input)
```

Hoặc tổng quát hơn:

```
secret = PRF(sk_webauthn, input)
```

Trong đó:
- `sk_webauthn`: WebAuthn private key (locked in authenticator)
- `input`: Salt/seed do bạn cung cấp (ví dụ: `"gun.sea.wrap.v1"`)
- `secret`: Output 32-bytes (256-bit)

### Tính chất quan trọng

#### ✅ **Deterministic (Xác định)**
Cùng input → Cùng secret

```javascript
// Lần 1
PRF(sk, "gun.sea.wrap.v1") 
// → 0x3a7f2c8d...  (32 bytes)

// Lần 2 (cùng passkey, cùng input)
PRF(sk, "gun.sea.wrap.v1")
// → 0x3a7f2c8d...  (GIỐNG HỆT!)

// Lần 3 (input khác)
PRF(sk, "different-salt")
// → 0x9b4e1f3a...  (KHÁC!)
```

#### ✅ **One-way (Một chiều)**
Không thể reverse engineer

```
secret → [có] → sk_webauthn  ❌ KHÔNG THỂ
sk_webauthn → [có] → secret  ✅ KHẢ NĂNG (nếu có passkey)
```

#### ✅ **Private Key Never Exposed**
Secret được tính **BÊN TRONG** authenticator

```
JavaScript land:
  ↓ request PRF
┌────────────────────┐
│  Authenticator     │
│  compute internally│  ← sk_webauthn stays here
│  secret = PRF(...) │
└────────────────────┘
  ↑ return secret only
JavaScript land:
  receives secret
```

---

## 🔐 Cryptographic Pipeline

### Full Flow: Từ Passkey → Encryption Key

```
┌──────────────────────────────────────────────────────────────┐
│ Step 1: PRF Evaluation                                       │
├──────────────────────────────────────────────────────────────┤
│ Input:    "gun.sea.wrap.v1" (your salt)                      │
│ Process:  PRF(sk_webauthn, input)                            │
│ Output:   secret [32 bytes]                                  │
│           e.g., 0x3a7f2c8d1e9b4f... (256 bits)              │
└──────────────────────────────────────────────────────────────┘
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ Step 2: HKDF Key Derivation                                  │
├──────────────────────────────────────────────────────────────┤
│ Algorithm: HKDF-SHA256                                        │
│ Input:     secret (from step 1)                              │
│ Salt:      "akaoproject gun.sea.wrap.v1"                     │
│ Info:      "gun.sea.wrap.kek.v1"                             │
│ Output:    KEK (Key Encryption Key) - AES-GCM-256            │
└──────────────────────────────────────────────────────────────┘
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ Step 3: AES-GCM Encryption                                   │
├──────────────────────────────────────────────────────────────┤
│ Algorithm: AES-GCM-256                                        │
│ Key:       KEK (from step 2)                                 │
│ IV:        Random 12 bytes (generated each time)             │
│ Plaintext: Gun SEA pair (JSON string)                        │
│ Output:    { iv, ciphertext }                                │
└──────────────────────────────────────────────────────────────┘
```

### Tại sao cần HKDF?

**HKDF** (HMAC-based Key Derivation Function) tăng cường bảo mật:

1. **Domain separation**: Salt & info đảm bảo keys khác nhau cho mục đích khác nhau
2. **Key stretching**: Output có entropy đủ cho AES-256
3. **Forward secrecy**: Compromise KEK không lộ PRF secret

```javascript
// Không dùng HKDF (❌ ít an toàn hơn)
const kek = secret  // Dùng trực tiếp

// Dùng HKDF (✅ best practice)
const kek = HKDF-SHA256(
  secret, 
  salt: "akaoproject gun.sea.wrap.v1",
  info: "gun.sea.wrap.kek.v1"
)
```

---

## 💻 Implementation Guide

### 1. Enable PRF Extension khi tạo Passkey

```javascript
const credential = await navigator.credentials.create({
  publicKey: {
    challenge: crypto.getRandomValues(new Uint8Array(32)),
    rp: { 
      id: "yourdomain.com", 
      name: "Your App" 
    },
    user: {
      id: userId,
      name: "username",
      displayName: "Display Name"
    },
    pubKeyCredParams: [
      { type: "public-key", alg: -7 }  // ECDSA P-256
    ],
    authenticatorSelection: {
      userVerification: "preferred",
      residentKey: "required"
    },
    // ⭐ QUAN TRỌNG: Enable PRF
    extensions: {
      prf: {}  // Empty object enables PRF support
    }
  }
})

// Check if PRF is enabled
const ext = credential.getClientExtensionResults()
console.log("PRF enabled:", ext.prf?.enabled)  // true/false
```

### 2. Get PRF Secret

```javascript
async function getPrfSecret(credentialId, salt = "gun.sea.wrap.v1") {
  const encoder = new TextEncoder()
  
  const assertion = await navigator.credentials.get({
    publicKey: {
      challenge: crypto.getRandomValues(new Uint8Array(32)),
      rpId: "yourdomain.com",
      allowCredentials: [{ 
        id: credentialId, 
        type: "public-key" 
      }],
      userVerification: "preferred",
      // ⭐ Request PRF evaluation
      extensions: {
        prf: {
          eval: {
            first: encoder.encode(salt)  // Your input
          }
        }
      }
    }
  })
  
  // Extract PRF result
  const ext = assertion.getClientExtensionResults()
  if (!ext?.prf?.results?.first) {
    throw new Error("PRF not supported or not enabled")
  }
  
  return new Uint8Array(ext.prf.results.first)  // 32 bytes
}
```

### 3. Derive Encryption Key (HKDF)

```javascript
async function deriveKey(prfSecret, info = "gun.sea.wrap.kek.v1") {
  const encoder = new TextEncoder()
  
  // Import PRF secret as base key
  const baseKey = await crypto.subtle.importKey(
    "raw",
    prfSecret,
    "HKDF",
    false,
    ["deriveKey"]
  )
  
  // Derive AES-GCM key
  const kek = await crypto.subtle.deriveKey(
    {
      name: "HKDF",
      hash: "SHA-256",
      salt: encoder.encode("akaoproject gun.sea.wrap.v1"),
      info: encoder.encode(info)
    },
    baseKey,
    { name: "AES-GCM", length: 256 },
    false,  // Non-extractable for security
    ["encrypt", "decrypt"]
  )
  
  return kek
}
```

### 4. Encrypt (Wrap) Gun Private Key

```javascript
async function wrapPrivateKey(credentialId, gunPair) {
  // Step 1: Get PRF secret
  const prfSecret = await getPrfSecret(credentialId)
  
  // Step 2: Derive encryption key
  const kek = await deriveKey(prfSecret)
  
  // Step 3: Encrypt Gun pair
  const encoder = new TextEncoder()
  const iv = crypto.getRandomValues(new Uint8Array(12))
  const plaintext = encoder.encode(JSON.stringify(gunPair))
  
  const ciphertext = await crypto.subtle.encrypt(
    { name: "AES-GCM", iv },
    kek,
    plaintext
  )
  
  return {
    iv: base64url(iv),
    ciphertext: base64url(new Uint8Array(ciphertext))
  }
}
```

### 5. Decrypt (Unwrap) Gun Private Key

```javascript
async function unwrapPrivateKey(credentialId, wrapped) {
  // Step 1: Get PRF secret (SAME as signup!)
  const prfSecret = await getPrfSecret(credentialId)
  
  // Step 2: Derive encryption key (SAME KEK!)
  const kek = await deriveKey(prfSecret)
  
  // Step 3: Decrypt
  const decoder = new TextDecoder()
  const iv = base64urlDecode(wrapped.iv)
  const ciphertext = base64urlDecode(wrapped.ciphertext)
  
  const plaintext = await crypto.subtle.decrypt(
    { name: "AES-GCM", iv },
    kek,
    ciphertext
  )
  
  return JSON.parse(decoder.decode(plaintext))
}
```

---

## 🌊 Complete Flow: Signup → Signin

### Scenario: E-commerce với Gun Database

#### **Signup Flow**

```javascript
// 1. User tạo passkey
const credential = await navigator.credentials.create({
  publicKey: {
    challenge: random(),
    user: { id: userId, name: "alice" },
    extensions: { prf: {} }  // Enable PRF
  }
})

// 2. Generate Gun pair (random, good entropy)
const gunPair = await SEA.pair()
// gunPair = {
//   pub: "~B7x8...",  // Gun public key
//   priv: "private",  // Gun private key
//   epub: "...",
//   epriv: "..."
// }

// 3. Wrap Gun pair với PRF
const wrapped = await wrapPrivateKey(credential.rawId, gunPair)
// wrapped = {
//   iv: "xT7pL9...",
//   ciphertext: "8fH2k..."
// }

// 4. Lưu wrapped key vào GunDB (public namespace, OK!)
const hash = await SEA.work(
  base64url(credential.rawId), 
  "wrapped-key"
)
gun.get('wrapped').get(hash).put(wrapped)

// 5. Lưu credential ID vào localStorage (để lookup sau)
localStorage.setItem('credential-id', base64url(credential.rawId))
```

**Dữ liệu lưu trữ:**

| Location | Data | Public? | Encrypted? |
|----------|------|---------|------------|
| Authenticator | `sk_webauthn` | ❌ Private | 🔒 Locked |
| GunDB | `wrapped = {iv, ciphertext}` | ✅ Public | ✅ Encrypted |
| localStorage | `credential-id` | ✅ Public | ❌ Plaintext (OK) |

---

#### **Signin Flow (Same Device)**

```javascript
// 1. Lấy credential ID từ localStorage
const credId = base64urlDecode(localStorage.getItem('credential-id'))

// 2. Authenticate với passkey
const assertion = await navigator.credentials.get({
  publicKey: {
    challenge: random(),
    allowCredentials: [{ id: credId, type: "public-key" }]
  }
})

// 3. Tính hash để lookup wrapped key
const hash = await SEA.work(
  base64url(credId), 
  "wrapped-key"
)

// 4. Lấy wrapped key từ GunDB
const wrapped = await gun.get('wrapped').get(hash).then()

// 5. Unwrap Gun pair bằng PRF
const gunPair = await unwrapPrivateKey(credId, wrapped)
// gunPair = {
//   pub: "~B7x8...",  // SAME as signup!
//   priv: "private",  // SAME as signup!
//   ...
// }

// 6. Login vào Gun
gun.user().auth(gunPair)

// ✅ User đã đăng nhập với cùng identity!
```

---

#### **Signin Flow (New Device - Multi-device)**

```javascript
// Scenario: User có cùng passkey đã sync qua iCloud/Google

// 1. User chọn "Sign in"

// 2. Authenticate (KHÔNG cần credential ID specific)
const assertion = await navigator.credentials.get({
  publicKey: {
    challenge: random(),
    // ⭐ Không có allowCredentials → browser shows all passkeys
  }
})

// 3. Tính hash từ credential ID đã chọn
const hash = await SEA.work(
  base64url(assertion.rawId), 
  "wrapped-key"
)

// 4. Lấy wrapped key từ GunDB
const wrapped = await gun.get('wrapped').get(hash).then()

if (!wrapped) {
  // Passkey mới, chưa có wrapped key
  return { error: "Please sign up first" }
}

// 5. Unwrap với passkey (works vì passkey đã sync!)
const gunPair = await unwrapPrivateKey(assertion.rawId, wrapped)

// 6. Login Gun
gun.user().auth(gunPair)

// ✅ Multi-device seamless!
```

---

## 🛡️ Security Analysis

### Threat Model

#### ❌ **Attack 1: Steal wrapped key từ GunDB**

```
Attacker: Lấy được { iv, ciphertext } từ GunDB
Can decrypt? NO ❌

Why?
→ Cần KEK để decrypt
→ KEK = HKDF(prfSecret)
→ prfSecret = PRF(sk_webauthn, salt)
→ sk_webauthn locked in authenticator
→ Cannot get prfSecret without passkey authentication
```

**Verdict**: ✅ Safe

---

#### ❌ **Attack 2: Man-in-the-middle intercept PRF secret**

```
Attacker: Intercept network traffic khi getPrfSecret()
Can get secret? NO ❌

Why?
→ PRF computation happens INSIDE authenticator
→ Communication authenticator ↔ browser uses secure channel
→ Even with MITM, PRF secret never transmitted in plaintext
→ WebAuthn uses TLS + platform security
```

**Verdict**: ✅ Safe (assuming no compromised browser/OS)

---

#### ❌ **Attack 3: Brute force ciphertext**

```
Attacker: Brute force AES-GCM-256 ciphertext
Success probability? ~0% ❌

Why?
→ AES-256: 2^256 possible keys
→ Time to brute force: ~10^59 years (with current tech)
→ AES-GCM also has authentication tag → detect tampering
```

**Verdict**: ✅ Safe (computationally infeasible)

---

#### ⚠️ **Attack 4: Phishing user để authenticate fake request**

```
Attacker: Lừa user authenticate trên fake site
Can get prfSecret? POTENTIALLY ⚠️

Scenario:
1. Fake site: "Please authenticate to continue"
2. User authenticates (thinking it's legit)
3. Attacker's site calls getPrfSecret()
4. Attacker gets prfSecret → can unwrap keys

Mitigation:
→ rpId must match origin
→ User education
→ Browser warnings for suspicious requests
```

**Verdict**: ⚠️ Social engineering risk (not crypto weakness)

---

#### ✅ **Attack 5: Compromised device**

```
Attacker: Malware on user's device
Can extract sk_webauthn? DIFFICULT ✅

Why?
→ sk_webauthn in TPM/Secure Enclave (hardware isolation)
→ Even root access cannot easily extract
→ Requires physical attack or firmware exploit

Note:
→ If device fully compromised, can intercept at runtime
→ But this is general device security issue, not PRF-specific
```

**Verdict**: ✅ Mitigated by hardware security

---

### Security Properties

| Property | Status | Notes |
|----------|--------|-------|
| Confidentiality | ✅ Strong | AES-GCM-256 |
| Integrity | ✅ Strong | GCM authentication tag |
| Forward Secrecy | ✅ Yes | Compromise KEK ≠ compromise sk |
| Key Rotation | ✅ Possible | Change salt → new PRF output |
| Multi-device | ✅ Native | Passkey sync via platform |
| User Presence | ✅ Required | Every unwrap needs auth |
| Phishing Resistant | ⚠️ Partial | rpId binding helps |

---

## 🆚 So sánh các phương pháp

### Method 1: Seed từ Passkey Public Key

```javascript
// ❌ Approach cũ
const seed = credential.pub  // "x.y" format
const pair = await SEA.pair(null, { seed })
```

**Vấn đề:**
- ❌ Phải lưu `credential.pub` somewhere
- ❌ Nếu lưu localStorage → không multi-device
- ❌ Nếu lưu GunDB plaintext → lộ seed
- ❌ Không thể encrypt vì chưa có pair (chicken-egg problem)

---

### Method 2: WebAuthn Signature (Không khả thi)

```javascript
// ❌ Idea: Dùng signature làm seed
const sig = await webauthn.sign({ data: "seed" })
const seed = sig.signature
const pair = await SEA.pair(null, { seed })
```

**Vấn đề:**
- ❌ Signature có random nonce → mỗi lần khác nhau
- ❌ Không deterministic → không thể recreate pair

---

### Method 3: PRF Extension ✅ (BEST)

```javascript
// ✅ Approach tối ưu
const prfSecret = await getPrfSecret(credentialId)
const kek = await deriveKey(prfSecret)
const wrapped = await encrypt(gunPair, kek)
gun.put(wrapped)  // Public, encrypted
```

**Ưu điểm:**
- ✅ Deterministic: Cùng passkey → cùng secret
- ✅ Private key không lộ
- ✅ Wrapped key có thể lưu public
- ✅ Multi-device native
- ✅ User presence required mỗi lần unwrap
- ✅ Best practice theo WebAuthn spec

---

## 📱 Multi-device Scenarios

### Scenario A: iCloud Keychain (Apple)

```
1. User signup trên iPhone
   → Passkey lưu trong iCloud Keychain
   → Wrapped key lưu trong GunDB

2. User mở MacBook (cùng Apple ID)
   → Passkey tự động sync qua iCloud
   → Browser thấy passkey available
   
3. User signin trên MacBook
   → Authenticate với passkey (Face ID)
   → getPrfSecret() → cùng secret!
   → Unwrap từ GunDB → cùng Gun pair!
   
✅ Seamless experience
```

---

### Scenario B: Google Password Manager

```
1. User signup trên Android (Chrome)
   → Passkey lưu trong Google Password Manager
   → Wrapped key lưu GunDB

2. User dùng Windows (Chrome, cùng Google account)
   → Passkey sync qua Google
   → Available in Chrome
   
3. User signin trên Windows
   → Authenticate với passkey
   → getPrfSecret() → cùng secret!
   → Unwrap successful
   
✅ Cross-platform works
```

---

### Scenario C: Hardware Security Key (YubiKey)

```
1. User signup với YubiKey
   → Passkey stored in YubiKey hardware
   → Wrapped key trong GunDB

2. User travel, dùng laptop khác
   → Cắm YubiKey vào laptop
   → Browser detect YubiKey
   
3. User signin
   → Authenticate with YubiKey (touch)
   → getPrfSecret() từ YubiKey
   → Unwrap successful
   
✅ Portable security
```

---

## 🚨 Common Pitfalls & Solutions

### Pitfall 1: PRF không được support

```javascript
// ❌ Không check support
const secret = await getPrfSecret(credId)
// → throws error nếu không support

// ✅ Check trước
const ext = credential.getClientExtensionResults()
if (!ext?.prf?.enabled) {
  console.error("PRF not supported on this device")
  // Fallback: Yêu cầu user dùng device khác
  // Hoặc: Implement backup method
}
```

**Browser support** (Jan 2026):
- Chrome/Edge: ✅ Full support
- Safari: ✅ iOS 17.4+, macOS 14+
- Firefox: ⚠️ Experimental flag
- Mobile: ✅ iOS/Android native support

---

### Pitfall 2: Salt không consistent

```javascript
// ❌ Salt khác nhau
// Signup
getPrfSecret(credId, "salt-v1")

// Signin
getPrfSecret(credId, "salt-v2")  // ← KHÁC!
// → Secret khác → KEK khác → decrypt FAIL!

// ✅ Dùng constant
const SALT = "gun.sea.wrap.v1"  // Global constant
getPrfSecret(credId, SALT)  // Luôn dùng same salt
```

---

### Pitfall 3: Quên lưu credential ID

```javascript
// ❌ Không lưu credential ID
await navigator.credentials.create(...)
// → User signin sau không biết dùng credential nào

// ✅ Lưu credential ID reference
const credential = await navigator.credentials.create(...)
const credIdB64 = base64url(credential.rawId)

// Option 1: localStorage
localStorage.setItem('cred-id', credIdB64)

// Option 2: Lưu trong GunDB (dưới user.name hoặc email)
gun.get('users').get(username).put({ credId: credIdB64 })
```

---

### Pitfall 4: IV reuse

```javascript
// ❌ Dùng cùng IV
const iv = new Uint8Array(12).fill(0)  // All zeros!
// → Catastrophic failure với AES-GCM

// ✅ Random IV mỗi lần
const iv = crypto.getRandomValues(new Uint8Array(12))
// → Mỗi encryption có IV unique
```

---

## 🔧 Advanced Topics

### Topic 1: Key Rotation

```javascript
// Scenario: User muốn đổi Gun pair (không đổi passkey)

async function rotateGunPair(credId) {
  // 1. Generate new Gun pair
  const newPair = await SEA.pair()
  
  // 2. Wrap with SAME passkey
  const wrapped = await wrapPrivateKey(credId, newPair)
  
  // 3. Update GunDB
  const hash = await SEA.work(base64url(credId), "wrapped-key")
  gun.get('wrapped').get(hash).put(wrapped)
  
  // 4. Migrate data từ old pair sang new pair
  // (tùy implementation)
}
```

---

### Topic 2: Multiple Passkeys per User

```javascript
// Scenario: User có nhiều passkeys (phone, laptop, YubiKey)

async function addSecondaryPasskey(primaryCredId, newPasskeyData) {
  // 1. Unwrap Gun pair bằng primary passkey
  const wrapped = await getWrappedKey(primaryCredId)
  const gunPair = await unwrapPrivateKey(primaryCredId, wrapped)
  
  // 2. Tạo new passkey
  const newCred = await navigator.credentials.create(newPasskeyData)
  
  // 3. Wrap CÙNG Gun pair bằng new passkey
  const newWrapped = await wrapPrivateKey(newCred.rawId, gunPair)
  
  // 4. Lưu wrapped key mới
  const hash = await SEA.work(base64url(newCred.rawId), "wrapped-key")
  gun.get('wrapped').get(hash).put(newWrapped)
  
  // ✅ User có thể signin bằng cả 2 passkeys!
}
```

---

### Topic 3: Backup & Recovery

```javascript
// Scenario: User mất device, muốn recovery

// Option 1: Recovery Passkey
// - User đăng ký recovery passkey lúc signup
// - Lưu recovery passkey riêng (paper backup, etc.)
// - Wrap Gun pair bằng recovery passkey

// Option 2: Social Recovery
// - Chia Gun pair thành shards (Shamir Secret Sharing)
// - Wrap mỗi shard bằng passkey của trusted contacts
// - Recovery cần N/M shards

// Option 3: Encrypted Backup
// - Export wrapped key + mật khẩu mạnh
// - Lưu encrypted backup offline
// - Import khi cần recovery

async function createRecoveryBackup(credId, password) {
  // Get Gun pair
  const wrapped = await getWrappedKey(credId)
  const gunPair = await unwrapPrivateKey(credId, wrapped)
  
  // Encrypt với password (PBKDF2)
  const salt = crypto.getRandomValues(new Uint8Array(16))
  const key = await crypto.subtle.importKey(
    "raw",
    new TextEncoder().encode(password),
    "PBKDF2",
    false,
    ["deriveKey"]
  )
  
  const encKey = await crypto.subtle.deriveKey(
    { name: "PBKDF2", salt, iterations: 100000, hash: "SHA-256" },
    key,
    { name: "AES-GCM", length: 256 },
    false,
    ["encrypt"]
  )
  
  const iv = crypto.getRandomValues(new Uint8Array(12))
  const ciphertext = await crypto.subtle.encrypt(
    { name: "AES-GCM", iv },
    encKey,
    new TextEncoder().encode(JSON.stringify(gunPair))
  )
  
  return {
    version: 1,
    salt: base64url(salt),
    iv: base64url(iv),
    ciphertext: base64url(new Uint8Array(ciphertext))
  }
  
  // User lưu backup này (QR code, file, paper wallet...)
}
```

---

## 📚 References & Resources

### Official Specifications

- [WebAuthn Level 3 Spec](https://w3c.github.io/webauthn/) - W3C Working Draft
- [PRF Extension Spec](https://w3c.github.io/webauthn/#prf-extension) - Section 10.5
- [CTAP 2.1 hmac-secret](https://fidoalliance.org/specs/fido-v2.1-ps-20210615/fido-client-to-authenticator-protocol-v2.1-ps-errata-20220621.html#sctn-hmac-secret-extension)

### Cryptography Standards

- [HKDF RFC 5869](https://datatracker.ietf.org/doc/html/rfc5869) - HMAC-based Key Derivation
- [AES-GCM NIST SP 800-38D](https://csrc.nist.gov/publications/detail/sp/800-38d/final) - Galois/Counter Mode

### Implementation Examples

- [WebAuthn.io](https://webauthn.io/) - Interactive demo
- [SimpleWebAuthn Library](https://github.com/MasterKale/SimpleWebAuthn) - TypeScript implementation
- [Auth0 WebAuthn](https://auth0.com/docs/secure/multi-factor-authentication/fido-authentication-with-webauthn) - Best practices

### Security Analysis

- [FIDO Alliance Security](https://fidoalliance.org/specs/common-specs/fido-security-ref-v2.0-ps-20190130.html)
- [WebAuthn Security Considerations](https://www.w3.org/TR/webauthn-3/#sctn-security-considerations)

---

## 💡 Best Practices Summary

### ✅ DO

1. **Always enable PRF extension** khi create passkey
2. **Use consistent salt** across signup/signin
3. **Generate random IV** cho mỗi encryption
4. **Store wrapped keys in GunDB** (public, encrypted OK)
5. **Check PRF support** trước khi implement
6. **Use HKDF** để derive keys (không dùng secret trực tiếp)
7. **Implement error handling** cho user cancellation
8. **Test multi-device flow** thoroughly
9. **Document recovery procedures** cho users
10. **Monitor browser support** for PRF extension

---

### ❌ DON'T

1. **Don't store sk_webauthn** - impossible anyway, but don't try
2. **Don't reuse IV** - catastrophic for AES-GCM
3. **Don't use PRF secret directly** as encryption key - use HKDF
4. **Don't store Gun pair plaintext** - always wrap first
5. **Don't assume PRF support** - check at runtime
6. **Don't use weak salts** - use domain-specific strings
7. **Don't ignore user education** - explain passkey concept
8. **Don't forget backup strategy** - users may lose devices
9. **Don't mix salt values** - maintain consistency
10. **Don't expose debugging info** in production - secrets in logs

---

## 🎓 Conclusion

WebAuthn PRF extension cung cấp một cách **an toàn, deterministic, và native multi-device** để wrap crypto keys mà không cần server. Đây là giải pháp tối ưu cho:

- 🔐 **Serverless authentication**
- 🌐 **Decentralized apps** (Gun, IPFS, etc.)
- 💰 **Crypto wallets** (non-custodial)
- 🔑 **Key management systems**

Key takeaways:

1. PRF = deterministic secret từ passkey
2. HKDF = derive encryption keys properly
3. AES-GCM = encrypt private data safely
4. Multi-device = native via passkey sync
5. Security = hardware-backed, user presence required

**Start implementing today!** 🚀

---

*Document version: 1.0*  
*Last updated: January 23, 2026*  
*Author: akaoio/akao project*
