import React from 'react';
import BasePage from '../../components/BasePage'
import { connect } from 'react-redux';
import { Tabs } from 'antd';
import CodeComponent from './_code';

class Page extends BasePage {
    componentDidMount() {
        this.props.dispatch({
            type: 'repository/getFolder',
            payload: {
                repositoryName: this.props.match.params.repository,
                flag: 'master',
                path: '/'
            }
        });
    }
    page() {
        return <Tabs>
            <Tabs.TabPane tab="Code" key="Code">
                <CodeComponent { ...this.props } />
            </Tabs.TabPane>
        </Tabs>
    }
}

export default connect(
    ({ repository }) => ({
        repositoryName: repository.focusName,
        files: repository.focusFolder
    })
)(Page);