import "/UI/layouts/main/index.js"
import "/UI/components/identicon/index.js"
import "/UI/components/avatars/index.js"
import "/UI/components/addresses/index.js"
import "/UI/components/authorize/index.js"
import "/UI/components/signout/index.js"
import "/UI/components/icon/index.js"
import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <header class="profile-hero">
            <div class="profile-hero__gradient"></div>
            <div class="profile-avatar" id="profile-avatar-wrap">
                <ui-identicon id="profile-identicon" data-size="11" />
                <button class="profile-avatar__edit" id="profile-avatar-edit" aria-label="Change avatar">
                    <ui-icon data-icon="pen" data-size="md" />
                </button>
            </div>
            <div class="profile-avatar-picker" id="profile-avatar-picker">
                <span class="profile-avatar-picker__loader" aria-hidden="true"></span>
                <ui-avatars />
            </div>
        </header>

        <div class="profile-body">
            <div class="profile-name-row">
                <div class="profile-name-block">
                    <span class="profile-name" id="profile-name"></span>
                    <button class="profile-name-edit" id="profile-name-edit" aria-label="Edit display name">
                        <ui-icon data-icon="pen" data-size="md" />
                    </button>
                    <input class="profile-name-input" id="profile-name-input" type="text" maxlength="64" autocomplete="off" />
                    <button class="profile-name-save" id="profile-name-save" aria-label="Save name">
                        <ui-icon data-icon="check-lg" data-size="md" />
                    </button>
                    <button class="profile-name-cancel" id="profile-name-cancel" aria-label="Cancel edit">
                        <ui-icon data-icon="x-lg" data-size="md" />
                    </button>
                </div>
                <div class="profile-auth-controls" id="profile-auth-controls">
                    <ui-authorize />
                    <ui-signout />
                </div>
            </div>

            <div class="profile-bio-row" id="profile-bio-row">
                <p class="profile-bio" id="profile-bio"></p>
                <button class="profile-bio-edit" id="profile-bio-edit" aria-label="Edit bio">
                    <ui-icon data-icon="pen" data-size="md" />
                </button>
                <div class="profile-bio-edit-wrap" id="profile-bio-edit-wrap">
                    <textarea class="profile-bio-input" id="profile-bio-input" maxlength="360" rows="3" placeholder="Write something about yourself…"></textarea>
                    <span class="profile-bio-count" id="profile-bio-count">360</span>
                </div>
                <button class="profile-bio-save" id="profile-bio-save" aria-label="Save bio">
                    <ui-icon data-icon="check-lg" data-size="md" />
                </button>
                <button class="profile-bio-cancel" id="profile-bio-cancel" aria-label="Cancel bio edit">
                    <ui-icon data-icon="x-lg" data-size="md" />
                </button>
            </div>

            <div class="profile-stats-row" id="profile-stats-row">
                <span class="profile-stat">
                    <span class="profile-stat__num" id="profile-following-count">0</span>
                    <span class="profile-stat__label">Following</span>
                </span>
            </div>

            <div class="profile-gate" id="profile-gate">
                <p>Sign in to view your profile.</p>
                <ui-authorize />
            </div>

            <div id="profile-sections">
                <section class="profile-section" id="profile-links-section">
                    <h2 class="profile-section__heading">Links</h2>
                    <div class="profile-links" id="profile-links-display"></div>
                    <div class="profile-links-edit-form" id="profile-links-edit-form"></div>
                    <div id="profile-links-actions">
                        <button class="profile-links-edit-btn" id="profile-links-edit">
                            <ui-icon data-icon="pen" data-size="md" /> Edit links
                        </button>
                    </div>
                    <div class="profile-links-save-row" id="profile-links-save-row">
                        <button class="profile-name-save" id="profile-links-save" aria-label="Save links">
                            <ui-icon data-icon="check-lg" data-size="md" />
                        </button>
                        <button class="profile-name-cancel" id="profile-links-cancel" aria-label="Cancel links edit">
                            <ui-icon data-icon="x-lg" data-size="md" />
                        </button>
                    </div>
                </section>

                <section class="profile-section">
                    <h2 class="profile-section__heading">Following</h2>
                    <div class="profile-following" id="profile-following-list"></div>
                    <form class="profile-following__add" id="profile-follow-form" autocomplete="off">
                        <input type="text" id="profile-follow-pub" placeholder="Paste 88-char public key" required />
                        <input type="text" id="profile-follow-name" placeholder="Display name" />
                        <button type="submit" class="profile-following__add-btn">Follow</button>
                    </form>
                </section>

                <section class="profile-section">
                    <h2 class="profile-section__heading">Addresses</h2>
                    <ui-addresses />
                </section>
            </div>
        </div>
    </layout-main>
`

export default template
