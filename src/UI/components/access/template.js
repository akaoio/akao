import "/UI/components/context/index.js"
import "/UI/components/modal/index.js"
import "/UI/components/button/index.js"
import "/UI/components/authenticate/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <ui-modal data-header="authentication" class="center">
        <main id="unauthenticated-screen">
            <h3><ui-context data-key="dictionary.welcome" /></h3>
            <p><ui-context data-key="dictionary.pleaseCreateAnAccountOrSignin" /></p>
            <div class="buttons">
                <ui-button class="full" data-left="fingerprint" id="signin">
                    <ui-context data-key="dictionary.signin" />
                </ui-button>
                <ui-button class="full" data-left="person-add" id="signup">
                    <ui-context data-key="dictionary.signup" />
                </ui-button>
            </div>
        </main>
        <main id="signin-screen">
            <h3><ui-context data-key="dictionary.signin" /></h3>
            <p><ui-context data-key="dictionary.pleaseChooseYourAuthenticationMethod" /></p>
            <ui-authenticate id="auth" />
        </main>
        <main id="signup-screen">
            <h3><ui-context data-key="dictionary.signup" /></h3>
            <form id="signup-form">
                <input type="text" name="name" />
                <input type="text" name="displayName" />
                <div class="buttons">
                    <ui-button class="full" data-left="check-lg" id="confirm">
                        <ui-context data-key="dictionary.confirm" />
                    </ui-button>
                    <ui-button class="full" data-left="arrow-left" id="back">
                        <ui-context data-key="dictionary.back" />
                    </ui-button>
                </div>
            </form>
        </main>
        <main id="sign-screen">
            <div class="buttons">
                <ui-button class="full" id="sign"><ui-context data-key="dictionary.sign" /></ui-button>
            </div>
        </main>
    </ui-modal>
`
export default template
